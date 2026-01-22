#pragma once

#include <string>

namespace video2ascii {

/**
 * @brief Класс для загрузки видео файлов по HTTP/HTTPS ссылкам
 */
class VideoDownloader {
public:
    /**
     * @brief Загружает видео файл по URL
     * @param url URL видео файла
     * @param outputPath Путь для сохранения файла
     * @return true если загрузка успешна, false иначе
     */
    static bool downloadFromUrl(const std::string& url, const std::string& outputPath);
    
    /**
     * @brief Проверяет наличие curl в системе
     * @return true если curl доступен, false иначе
     */
    static bool isCurlAvailable();

private:
    /**
     * @brief Формирует команду curl для загрузки
     * @param url URL для загрузки
     * @param outputPath Путь для сохранения
     * @return Команда curl
     */
    static std::string buildCurlCommand(const std::string& url, const std::string& outputPath);
};

} // namespace video2ascii
