#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace video2ascii {

/**
 * @brief Класс для кэширования загруженных видео
 */
class VideoCache {
public:
    /**
     * @brief Получить путь к кэшированному файлу по URL
     * @param url URL видео
     * @return Путь к кэшированному файлу или пустая строка если не найден
     */
    static std::string getCachedPath(const std::string& url);
    
    /**
     * @brief Сохранить файл в кэш
     * @param url URL видео
     * @param filePath Путь к загруженному файлу
     * @return Путь к файлу в кэше
     */
    static std::string cacheFile(const std::string& url, const std::string& filePath);
    
    /**
     * @brief Очистить кэш
     */
    static void clearCache();
    
    /**
     * @brief Получить директорию кэша
     */
    static std::string getCacheDirectory();
    
    /**
     * @brief Проверить, существует ли файл в кэше
     */
    static bool isCached(const std::string& url);

private:
    static std::string urlToHash(const std::string& url);
    static std::string getCacheDir();
    static bool ensureCacheDirectory();
};

} // namespace video2ascii
