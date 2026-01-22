#include "video_processor.h"
#include "utils.h"
#include "logger.h"
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <algorithm>

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace video2ascii {

VideoProcessor::VideoProcessor(const std::string& videoPath, 
                              int targetWidth,
                              const std::string& asciiChars,
                              config::OutputMode outputMode,
                              const std::string& outputFile,
                              bool colored)
    : asciiChars_(asciiChars),
      width_(targetWidth), height_(0), fps_(0), frameDurationMs_(0),
      isOpen_(false), colored_(colored), outputMode_(outputMode),
      outputFilePath_(outputFile),
      maxFramesInMemory_(config::MAX_FRAMES_IN_MEMORY) {
    
    // Инициализация статистики
    stats_.reset();
    
    // Открываем видео с улучшенной обработкой ошибок
    handleOpenCVError("открытие видео");
    cap_.open(videoPath);
    
    if (!cap_.isOpened()) {
        std::string errorMsg = "Не удалось открыть видео: " + videoPath;
        errorMsg += "\nВозможные причины:";
        errorMsg += "\n  - Файл не существует или поврежден";
        errorMsg += "\n  - Неподдерживаемый формат видео";
        errorMsg += "\n  - Отсутствуют необходимые кодеки";
        throw std::runtime_error(errorMsg);
    }
    
    isOpen_ = true;
    
    // Получаем свойства видео
    fps_ = cap_.get(cv::CAP_PROP_FPS);
    if (fps_ <= 0) {
        LOG_WARNING("Не удалось определить FPS, используется значение по умолчанию: " + 
                   std::to_string(config::DEFAULT_FPS));
        fps_ = config::DEFAULT_FPS;
    }
    
    frameDurationMs_ = static_cast<int>(1000.0 / fps_);
    
    int frameWidth = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
    
    if (frameWidth <= 0 || frameHeight <= 0) {
        throw std::runtime_error("Неверные размеры видео: " + 
                                 std::to_string(frameWidth) + "x" + 
                                 std::to_string(frameHeight));
    }
    
    calculateDimensions(targetWidth, frameWidth, frameHeight);
    
    // Инициализируем AsciiCache с переданными символами
    asciiCache_.setChars(asciiChars_);
    
    // Открываем файл для вывода если нужно
    if (outputMode_ == config::OutputMode::FILE || 
        outputMode_ == config::OutputMode::BOTH) {
        if (outputFilePath_.empty()) {
            outputFilePath_ = "output_ascii.txt";
        }
        outputFile_.open(outputFilePath_, std::ios::out);
        if (!outputFile_.is_open()) {
            LOG_WARNING("Не удалось открыть файл для записи: " + outputFilePath_);
            outputMode_ = config::OutputMode::CONSOLE;
        }
    }
    
    LOG_INFO("Видео открыто: " + videoPath);
    LOG_INFO("Размер: " + std::to_string(frameWidth) + "x" + std::to_string(frameHeight));
    LOG_INFO("FPS: " + std::to_string(fps_));
    LOG_INFO("ASCII размер: " + std::to_string(width_) + "x" + std::to_string(height_));
}

VideoProcessor::~VideoProcessor() {
    stop();
    
    if (isOpen_) {
        cap_.release();
    }
    
    if (outputFile_.is_open()) {
        outputFile_.close();
    }
    
    stats_.endTime = std::chrono::steady_clock::now();
}

void VideoProcessor::calculateDimensions(int targetWidth, int originalWidth, int originalHeight) {
    width_ = targetWidth;
    height_ = static_cast<int>((targetWidth * originalHeight / originalWidth) * config::HEIGHT_RATIO);
}

bool VideoProcessor::isValidFrame(const cv::Mat& frame) const {
    if (frame.empty()) {
        return false;
    }
    
    if (frame.rows <= 0 || frame.cols <= 0) {
        return false;
    }
    
    if (frame.data == nullptr) {
        return false;
    }
    
    return true;
}

std::string VideoProcessor::frameToAscii(const cv::Mat& frame) {
    if (!isValidFrame(frame)) {
        return "";
    }
    
    std::string asciiFrame;
    asciiFrame.reserve(width_ * height_ + height_);
    
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            if (i < frame.rows && j < frame.cols) {
                asciiFrame += asciiCache_.pixelToASCII(frame.at<uchar>(i, j));
            } else {
                asciiFrame += ' ';
            }
        }
        asciiFrame += "\n";
    }
    
    return asciiFrame;
}

std::string VideoProcessor::frameToColoredAscii(const cv::Mat& frame) {
    if (!isValidFrame(frame)) {
        return "";
    }
    
    std::string asciiFrame;
    asciiFrame.reserve(width_ * height_ * 20); // Больше места для ANSI кодов
    
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            if (i < frame.rows && j < frame.cols) {
                cv::Vec3b pixel = frame.at<cv::Vec3b>(i, j);
                int b = pixel[0];
                int g = pixel[1];
                int r = pixel[2];
                
                // Вычисляем яркость
                int intensity = static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b);
                char asciiChar = asciiCache_.pixelToASCII(intensity);
                
                // ANSI цветовой код
                asciiFrame += "\033[38;2;" + std::to_string(r) + ";" + 
                             std::to_string(g) + ";" + std::to_string(b) + "m";
                asciiFrame += asciiChar;
                asciiFrame += "\033[0m"; // Сброс цвета
            } else {
                asciiFrame += ' ';
            }
        }
        asciiFrame += "\n";
    }
    
    return asciiFrame;
}

void VideoProcessor::handleOpenCVError(const std::string& operation) {
    // OpenCV автоматически обрабатывает ошибки через исключения
    // Здесь можно добавить дополнительную логику если нужно
    (void)operation;
}

void VideoProcessor::processingThread() {
    try {
        cv::Mat frame, processedFrame, resizedFrame;
        stats_.startTime = std::chrono::steady_clock::now();
        
        while (!stopped_.load() && !SignalHandler::shouldExit()) {
            cap_ >> frame;
            
            if (frame.empty()) {
                break;
            }
            
            // Проверяем валидность кадра
            if (!isValidFrame(frame)) {
                stats_.framesSkipped++;
                LOG_DEBUG("Пропущен невалидный кадр");
                continue;
            }
            
            // Обрабатываем кадр
            try {
                if (colored_) {
                    cv::resize(frame, resizedFrame, cv::Size(width_, height_), 0, 0, cv::INTER_LINEAR);
                    processedFrame = resizedFrame;
                } else {
                    cv::cvtColor(frame, processedFrame, cv::COLOR_BGR2GRAY);
                    cv::resize(processedFrame, resizedFrame, cv::Size(width_, height_), 0, 0, cv::INTER_LINEAR);
                    processedFrame = resizedFrame;
                }
                
                std::string asciiFrame = colored_ ? 
                    frameToColoredAscii(processedFrame) : 
                    frameToAscii(processedFrame);
                
                if (asciiFrame.empty()) {
                    stats_.framesSkipped++;
                    continue;
                }
                
                // Ограничение памяти - удаляем старые кадры
                {
                    std::lock_guard<std::mutex> lock(framesMutex_);
                    
                    // Если буфер переполнен, удаляем старые кадры
                    while (asciiFrames_.size() >= maxFramesInMemory_) {
                        asciiFrames_.erase(asciiFrames_.begin());
                        framesInBuffer_--;
                    }
                    
                    asciiFrames_.push_back(std::move(asciiFrame));
                    framesInBuffer_ = asciiFrames_.size();
                    
                    // Обновляем статистику памяти (примерная оценка)
                    size_t estimatedMB = (framesInBuffer_.load() * width_ * height_) / (1024 * 1024);
                    stats_.memoryUsedMB = estimatedMB;
                }
                
                stats_.framesProcessed++;
                framesCv_.notify_one();
                
            } catch (const std::exception& e) {
                stats_.framesSkipped++;
                LOG_WARNING("Ошибка обработки кадра: " + std::string(e.what()));
                continue;
            }
        }
        
        {
            std::lock_guard<std::mutex> lock(framesMutex_);
            processingComplete_ = true;
        }
        framesCv_.notify_all();
        
    } catch (const std::exception& e) {
        processingError_ = true;
        processingErrorMessage_ = e.what();
        LOG_ERROR("Критическая ошибка в потоке обработки: " + std::string(e.what()));
        
        {
            std::lock_guard<std::mutex> lock(framesMutex_);
            processingComplete_ = true;
        }
        framesCv_.notify_all();
    }
}

void VideoProcessor::playbackThread() {
    try {
        size_t frameIndex = 0;
        
        while (!stopped_.load() && !SignalHandler::shouldExit()) {
            // Обработка паузы
            while (paused_.load() && !stopped_.load() && !SignalHandler::shouldExit()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            if (stopped_.load() || SignalHandler::shouldExit()) {
                break;
            }
            
            std::unique_lock<std::mutex> lock(framesMutex_);
            
            framesCv_.wait(lock, [this]() {
                return !asciiFrames_.empty() || processingComplete_ || stopped_.load();
            });
            
            if (stopped_.load() || SignalHandler::shouldExit()) {
                break;
            }
            
            if (frameIndex < asciiFrames_.size()) {
                std::string frameToShow = asciiFrames_[frameIndex];
                lock.unlock();
                
                outputFrame(frameToShow, frameIndex);
                
                stats_.framesRendered++;
                frameIndex++;
                
                std::this_thread::sleep_for(std::chrono::milliseconds(frameDurationMs_));
            } else if (processingComplete_) {
                playbackComplete_ = true;
                break;
            }
        }
        
        stats_.endTime = std::chrono::steady_clock::now();
        
    } catch (const std::exception& e) {
        LOG_ERROR("Критическая ошибка в потоке воспроизведения: " + std::string(e.what()));
        playbackComplete_ = true;
    }
}

void VideoProcessor::outputFrame(const std::string& frame, size_t frameIndex) {
    if (outputMode_ == config::OutputMode::CONSOLE || 
        outputMode_ == config::OutputMode::BOTH) {
        utils::clearScreen();
        std::cout << frame;
        std::cout.flush();
    }
    
    if (outputMode_ == config::OutputMode::FILE || 
        outputMode_ == config::OutputMode::BOTH) {
        if (outputFile_.is_open()) {
            outputFile_ << "=== Frame " << frameIndex << " ===\n";
            outputFile_ << frame << "\n\n";
            outputFile_.flush();
        }
    }
}

void VideoProcessor::handleUserInput() {
    // Неблокирующая проверка ввода (упрощенная версия)
    // В полной реализации можно использовать select/poll
    // Здесь просто проверяем флаги
    if (SignalHandler::shouldExit()) {
        stop();
    }
}

void VideoProcessor::process() {
    if (processingError_) {
        throw std::runtime_error("Ошибка обработки: " + processingErrorMessage_);
    }
    
    std::thread processingThread(&VideoProcessor::processingThread, this);
    std::thread playbackThread(&VideoProcessor::playbackThread, this);
    std::thread inputThread([this]() {
        while (!stopped_.load() && !playbackComplete_.load()) {
            handleUserInput();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    
    processingThread.join();
    playbackThread.join();
    inputThread.join();
    
    if (outputMode_ == config::OutputMode::FILE || 
        outputMode_ == config::OutputMode::BOTH) {
        LOG_INFO("ASCII арт сохранен в файл: " + outputFilePath_);
    }
    
    stats_.print();
}

void VideoProcessor::pause() {
    paused_ = true;
    LOG_INFO("Воспроизведение приостановлено");
}

void VideoProcessor::resume() {
    paused_ = false;
    LOG_INFO("Воспроизведение возобновлено");
}

void VideoProcessor::stop() {
    stopped_ = true;
    paused_ = false;
    framesCv_.notify_all();
    LOG_INFO("Обработка остановлена");
}

} // namespace video2ascii
