#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "ascii_cache.h"
#include "config.h"

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
     * @throw std::runtime_error если не удалось открыть видео
     */
    explicit VideoProcessor(const std::string& videoPath, int targetWidth = config::DEFAULT_WIDTH);
    
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
    
    // Геттеры
    bool isVideoOpen() const { return isOpen_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    double getFps() const { return fps_; }

private:
    /**
     * @brief Вычисляет размеры ASCII-изображения с сохранением пропорций
     */
    void calculateDimensions(int targetWidth, int originalWidth, int originalHeight);
    
    /**
     * @brief Преобразует кадр в ASCII строку
     * @param frame Кадр в градациях серого
     * @return ASCII представление кадра
     */
    std::string frameToAscii(const cv::Mat& frame);
    
    /**
     * @brief Поток обработки кадров
     */
    void processingThread();
    
    /**
     * @brief Поток воспроизведения кадров
     */
    void playbackThread();

private:
    cv::VideoCapture cap_;
    AsciiCache asciiCache_;
    int width_;
    int height_;
    double fps_;
    int frameDurationMs_;
    bool isOpen_;
    
    std::vector<std::string> asciiFrames_;
    std::mutex framesMutex_;
    std::condition_variable framesCv_;
    bool processingComplete_;
    bool playbackComplete_;
};

} // namespace video2ascii
