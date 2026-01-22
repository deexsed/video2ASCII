#include "video_validator.h"
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

namespace video2ascii {

const std::vector<std::string> VideoValidator::supportedExtensions_ = {
    ".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm", ".m4v",
    ".3gp", ".3g2", ".asf", ".rm", ".rmvb", ".vob", ".ogv", ".mpeg",
    ".mpg", ".m2v", ".ts", ".mts", ".m2ts", ".divx", ".xvid", ".f4v"
};

std::string VideoValidator::getFileExtension(const std::string& filepath) {
    fs::path path(filepath);
    std::string ext = path.extension().string();
    
    // Приводим к нижнему регистру
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    return ext;
}

bool VideoValidator::hasValidExtension(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    return std::find(supportedExtensions_.begin(), supportedExtensions_.end(), ext) 
           != supportedExtensions_.end();
}

bool VideoValidator::isValidVideoFile(const std::string& filepath) {
    // Проверяем существование файла
    if (!fs::exists(filepath)) {
        return false;
    }
    
    // Проверяем, что это файл, а не директория
    if (!fs::is_regular_file(filepath)) {
        return false;
    }
    
    // Проверяем расширение
    if (!hasValidExtension(filepath)) {
        return false;
    }
    
    // Проверяем размер файла (должен быть больше 0)
    if (fs::file_size(filepath) == 0) {
        return false;
    }
    
    return true;
}

std::vector<std::string> VideoValidator::getSupportedExtensions() {
    return supportedExtensions_;
}

bool VideoValidator::hasValidMimeType(const std::string& filepath) {
    // Простая проверка по расширению
    // В реальном приложении можно использовать библиотеку для определения MIME-типа
    return hasValidExtension(filepath);
}

} // namespace video2ascii
