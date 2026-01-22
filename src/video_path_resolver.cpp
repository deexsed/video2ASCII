#include "video_path_resolver.h"
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

namespace video2ascii {

std::string VideoPathResolver::downloadVideo(const std::string& url) {
    std::string tempPath = utils::generateTempFileName(".mp4");
    
    if (!VideoDownloader::downloadFromUrl(url, tempPath)) {
        throw std::runtime_error("Не удалось загрузить видео по ссылке");
    }
    
    return tempPath;
}

void VideoPathResolver::validateLocalFile(const std::string& path) {
    if (!fs::exists(path)) {
        throw std::runtime_error("Файл не найден: " + path);
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
