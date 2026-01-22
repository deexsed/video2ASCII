#include "video_path_resolver.h"
#include "logger.h"
#include "config.h"
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

namespace video2ascii {

std::string VideoPathResolver::downloadVideo(const std::string& url) {
    // Проверяем кэш
    std::string cachedPath = VideoCache::getCachedPath(url);
    if (!cachedPath.empty()) {
        LOG_INFO("Используется кэшированное видео: " + cachedPath);
        return cachedPath;
    }
    
    std::string tempPath = utils::generateTempFileName(".mp4");
    
    // Callback для прогресса
    auto progressCallback = [](size_t downloaded, size_t total) {
        VideoDownloader::showProgressBar(downloaded, total);
    };
    
    if (!VideoDownloader::downloadFromUrl(url, tempPath, 
                                         config::DOWNLOAD_TIMEOUT_SECONDS,
                                         progressCallback)) {
        throw std::runtime_error("Не удалось загрузить видео по ссылке");
    }
    
    // Сохраняем в кэш
    std::string cached = VideoCache::cacheFile(url, tempPath);
    if (cached != tempPath) {
        // Удаляем временный файл, используем кэшированный
        try {
            fs::remove(tempPath);
        } catch (...) {
            // Игнорируем ошибки удаления
        }
        return cached;
    }
    
    return tempPath;
}

void VideoPathResolver::validateLocalFile(const std::string& path) {
    if (!fs::exists(path)) {
        throw std::runtime_error("Файл не найден: " + path);
    }
    
    if (!VideoValidator::isValidVideoFile(path)) {
        throw std::runtime_error("Файл не является валидным видео: " + path);
    }
}

std::string VideoPathResolver::resolve(const std::string& input) {
    if (utils::isUrl(input)) {
        return downloadVideo(input);
    }
    
    validateLocalFile(input);
    return input;
}

bool VideoPathResolver::isTemporaryFile(const std::string& path) {
    return path.find("temp_video_") == 0;
}

} // namespace video2ascii
