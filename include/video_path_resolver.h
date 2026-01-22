#pragma once

#include <string>
#include "video_downloader.h"
#include "utils.h"

namespace video2ascii {

/**
 * @brief Класс для разрешения пути к видео (локальный файл или URL)
 */
class VideoPathResolver {
public:
    /**
     * @brief Получает путь к видео файлу (загружает если это URL)
     * @param input Путь к локальному файлу или URL
     * @return Путь к видео файлу (локальный или временный загруженный)
     * @throw std::runtime_error если не удалось загрузить или файл не найден
     */
    static std::string resolve(const std::string& input);
    
    /**
     * @brief Проверяет, является ли путь временным файлом (загруженным по URL)
     * @param path Путь для проверки
     * @return true если это временный файл
     */
    static bool isTemporaryFile(const std::string& path);

private:
    /**
     * @brief Загружает видео по URL и возвращает путь к временному файлу
     * @param url URL видео
     * @return Путь к загруженному временному файлу
     */
    static std::string downloadVideo(const std::string& url);
    
    /**
     * @brief Проверяет существование локального файла
     * @param path Путь к файлу
     * @throw std::runtime_error если файл не найден
     */
    static void validateLocalFile(const std::string& path);
};

} // namespace video2ascii
