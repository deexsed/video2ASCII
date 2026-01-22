#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <fstream>
#include "ascii_cache.h"
#include "config.h"
#include "processing_stats.h"
#include "signal_handler.h"

namespace video2ascii {

/**
 * @brief Класс для обработки видео и преобразования в ASCII-арт (RAII)
 * 
 * Использует многопоточность для параллельной обработки кадров
 * и их воспроизведения в консоли.
 */
class VideoProcessor {
public:
    /**
     * @brief Конструктор - открывает видео файл
     * @param videoPath Путь к видео файлу
     * @param targetWidth Ширина ASCII-изображения
     * @param asciiChars Набор ASCII символов для отрисовки
     * @param outputMode Режим вывода (консоль, файл, оба)
     * @param outputFile Путь к файлу для сохранения (если outputMode включает FILE)
     * @param colored Использовать ли цветной ASCII
     * @throw std::runtime_error если не удалось открыть видео
     */
    explicit VideoProcessor(const std::string& videoPath, 
                          int targetWidth = config::DEFAULT_WIDTH,
                          const std::string& asciiChars = config::ascii_styles::BASIC,
                          config::OutputMode outputMode = config::OutputMode::CONSOLE,
                          const std::string& outputFile = "",
                          bool colored = false);
    
    /**
     * @brief Деструктор - освобождает ресурсы
     */
    ~VideoProcessor();
    
    // Удаляем копирование (Rule of Five)
    VideoProcessor(const VideoProcessor&) = delete;
    VideoProcessor& operator=(const VideoProcessor&) = delete;
    VideoProcessor(VideoProcessor&&) = delete;
    VideoProcessor& operator=(VideoProcessor&&) = delete;
    
    /**
     * @brief Запускает обработку и воспроизведение видео
     */
    void process();
    
    /**
     * @brief Приостановить воспроизведение
     */
    void pause();
    
    /**
     * @brief Возобновить воспроизведение
     */
    void resume();
    
    /**
     * @brief Остановить обработку
     */
    void stop();
    
    /**
     * @brief Получить статистику обработки
     */
    const ProcessingStats& getStats() const { return stats_; }
    
    // Геттеры
    bool isVideoOpen() const { return isOpen_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    double getFps() const { return fps_; }
    bool isPaused() const { return paused_.load(); }
    bool isStopped() const { return stopped_.load(); }

private:
    /**
     * @brief Вычисляет размеры ASCII-изображения с сохранением пропорций
     */
    void calculateDimensions(int targetWidth, int originalWidth, int originalHeight);
    
    /**
     * @brief Преобразует кадр в ASCII строку
     * @param frame Кадр в градациях серого или цветной
     * @return ASCII представление кадра
     */
    std::string frameToAscii(const cv::Mat& frame);
    
    /**
     * @brief Преобразует цветной кадр в цветной ASCII
     * @param frame Цветной кадр
     * @return Цветное ASCII представление кадра
     */
    std::string frameToColoredAscii(const cv::Mat& frame);
    
    /**
     * @brief Проверяет валидность кадра
     */
    bool isValidFrame(const cv::Mat& frame) const;
    
    /**
     * @brief Поток обработки кадров
     */
    void processingThread();
    
    /**
     * @brief Поток воспроизведения кадров
     */
    void playbackThread();
    
    /**
     * @brief Обработка пользовательского ввода (неблокирующая)
     */
    void handleUserInput();
    
    /**
     * @brief Вывод кадра в консоль или файл
     */
    void outputFrame(const std::string& frame, size_t frameIndex);
    
    /**
     * @brief Улучшенная обработка ошибок OpenCV
     */
    void handleOpenCVError(const std::string& operation);

private:
    cv::VideoCapture cap_;
    AsciiCache asciiCache_;
    std::string asciiChars_;
    int width_;
    int height_;
    double fps_;
    int frameDurationMs_;
    bool isOpen_;
    bool colored_;
    config::OutputMode outputMode_;
    std::string outputFilePath_;
    std::ofstream outputFile_;
    
    std::vector<std::string> asciiFrames_;
    std::mutex framesMutex_;
    std::condition_variable framesCv_;
    std::atomic<bool> processingComplete_{false};
    std::atomic<bool> playbackComplete_{false};
    std::atomic<bool> paused_{false};
    std::atomic<bool> stopped_{false};
    std::atomic<bool> processingError_{false};
    std::string processingErrorMessage_;
    
    ProcessingStats stats_;
    
    // Для ограничения памяти
    size_t maxFramesInMemory_;
    std::atomic<size_t> framesInBuffer_{0};
};

} // namespace video2ascii
