#pragma once

#include <string>
#include <functional>

namespace video2ascii {

/**
 * @brief Callback для прогресса загрузки
 * @param downloaded Загружено байт
 * @param total Всего байт (0 если неизвестно)
 */
using ProgressCallback = std::function<void(size_t downloaded, size_t total)>;

/**
 * @brief Класс для загрузки видео файлов по HTTP/HTTPS ссылкам
 */
class VideoDownloader {
public:
    /**
     * @brief Загружает видео файл по URL
     * @param url URL видео файла
     * @param outputPath Путь для сохранения файла
     * @param timeoutSeconds Таймаут в секундах (0 = без таймаута)
     * @param progressCallback Callback для отображения прогресса
     * @return true если загрузка успешна, false иначе
     */
    static bool downloadFromUrl(const std::string& url, 
                                const std::string& outputPath,
                                int timeoutSeconds = 300,
                                ProgressCallback progressCallback = nullptr);
    
    /**
     * @brief Проверяет наличие curl в системе
     * @return true если curl доступен, false иначе
     */
    static bool isCurlAvailable();
    
    /**
     * @brief Отображает прогресс-бар
     * @param downloaded Загружено байт
     * @param total Всего байт
     */
    static void showProgressBar(size_t downloaded, size_t total);

private:
    /**
     * @brief Формирует команду curl для загрузки
     * @param url URL для загрузки
     * @param outputPath Путь для сохранения
     * @param timeoutSeconds Таймаут в секундах
     * @param showProgress Показывать ли прогресс
     * @return Команда curl
     */
    static std::string buildCurlCommand(const std::string& url, 
                                       const std::string& outputPath,
                                       int timeoutSeconds,
                                       bool showProgress);
};

} // namespace video2ascii
