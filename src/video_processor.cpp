#include "video_processor.h"
#include "utils.h"
#include <iostream>
#include <chrono>
#include <stdexcept>

namespace video2ascii {

VideoProcessor::VideoProcessor(const std::string& videoPath, int targetWidth)
    : width_(targetWidth), height_(0), fps_(0), frameDurationMs_(0),
      isOpen_(false), processingComplete_(false), playbackComplete_(false) {
    
    cap_.open(videoPath);
    if (!cap_.isOpened()) {
        throw std::runtime_error("Не удалось открыть видео: " + videoPath);
    }
    
    isOpen_ = true;
    fps_ = cap_.get(cv::CAP_PROP_FPS);
    if (fps_ <= 0) {
        fps_ = config::DEFAULT_FPS;
    }
    
    frameDurationMs_ = static_cast<int>(1000.0 / fps_);
    
    int frameWidth = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
    
    if (frameWidth <= 0 || frameHeight <= 0) {
        throw std::runtime_error("Неверные размеры видео");
    }
    
    calculateDimensions(targetWidth, frameWidth, frameHeight);
}

VideoProcessor::~VideoProcessor() {
    if (isOpen_) {
        cap_.release();
    }
}

void VideoProcessor::calculateDimensions(int targetWidth, int originalWidth, int originalHeight) {
    width_ = targetWidth;
    height_ = static_cast<int>((targetWidth * originalHeight / originalWidth) * config::HEIGHT_RATIO);
}

std::string VideoProcessor::frameToAscii(const cv::Mat& frame) {
    std::string asciiFrame;
    asciiFrame.reserve(width_ * height_ + height_); // Предварительное выделение памяти
    
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            asciiFrame += asciiCache_.pixelToASCII(frame.at<uchar>(i, j));
        }
        asciiFrame += "\n";
    }
    return asciiFrame;
}

void VideoProcessor::processingThread() {
    cv::Mat frame, grayFrame, resizedFrame;
    
    while (true) {
        cap_ >> frame;
        if (frame.empty()) {
            break;
        }
        
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::resize(grayFrame, resizedFrame, cv::Size(width_, height_), 0, 0, cv::INTER_LINEAR);
        
        std::string asciiFrame = frameToAscii(resizedFrame);
        
        {
            std::lock_guard<std::mutex> lock(framesMutex_);
            asciiFrames_.push_back(std::move(asciiFrame));
        }
        framesCv_.notify_one();
    }
    
    {
        std::lock_guard<std::mutex> lock(framesMutex_);
        processingComplete_ = true;
    }
    framesCv_.notify_all();
}

void VideoProcessor::playbackThread() {
    size_t frameIndex = 0;
    
    while (true) {
        std::unique_lock<std::mutex> lock(framesMutex_);
        
        // Ждем новый кадр или завершения обработки
        // Проверяем размер вектора внутри условия
        framesCv_.wait(lock, [this]() {
            return !asciiFrames_.empty() || processingComplete_;
        });
        
        // Если есть кадры для воспроизведения
        if (frameIndex < asciiFrames_.size()) {
            std::string frameToShow = asciiFrames_[frameIndex];
            lock.unlock();
            
            utils::clearScreen();
            std::cout << frameToShow;
            std::this_thread::sleep_for(std::chrono::milliseconds(frameDurationMs_));
            
            frameIndex++;
        } else if (processingComplete_) {
            // Все кадры обработаны и воспроизведены
            playbackComplete_ = true;
            break;
        }
    }
}

void VideoProcessor::process() {
    std::thread processingThread(&VideoProcessor::processingThread, this);
    std::thread playbackThread(&VideoProcessor::playbackThread, this);
    
    processingThread.join();
    playbackThread.join();
}

} // namespace video2ascii
