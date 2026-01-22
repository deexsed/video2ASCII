#include "video_cache.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

// Упрощенная версия без OpenSSL (используем простой хэш)
namespace {
    std::string simpleHash(const std::string& str) {
        size_t hash = 0;
        for (char c : str) {
            hash = ((hash << 5) - hash) + c;
            hash = hash & hash; // Преобразуем в size_t
        }
        std::ostringstream oss;
        oss << std::hex << hash;
        return oss.str();
    }
}

namespace fs = std::filesystem;

namespace video2ascii {

std::string VideoCache::getCacheDirectory() {
    return getCacheDir();
}

std::string VideoCache::getCacheDir() {
    const char* home = std::getenv("HOME");
    if (!home) {
        home = std::getenv("USERPROFILE"); // Windows
    }
    if (!home) {
        return ".video2ascii_cache";
    }
    return std::string(home) + "/.video2ascii_cache";
}

bool VideoCache::ensureCacheDirectory() {
    std::string cacheDir = getCacheDir();
    if (!fs::exists(cacheDir)) {
        try {
            fs::create_directories(cacheDir);
            return true;
        } catch (...) {
            return false;
        }
    }
    return fs::is_directory(cacheDir);
}

std::string VideoCache::urlToHash(const std::string& url) {
    return simpleHash(url);
}

std::string VideoCache::getCachedPath(const std::string& url) {
    if (!ensureCacheDirectory()) {
        return "";
    }
    
    std::string hash = urlToHash(url);
    std::string cacheDir = getCacheDir();
    std::string cachedPath = cacheDir + "/" + hash + ".mp4";
    
    if (fs::exists(cachedPath) && fs::is_regular_file(cachedPath)) {
        if (fs::file_size(cachedPath) > 0) {
            return cachedPath;
        }
    }
    
    return "";
}

bool VideoCache::isCached(const std::string& url) {
    return !getCachedPath(url).empty();
}

std::string VideoCache::cacheFile(const std::string& url, const std::string& filePath) {
    if (!ensureCacheDirectory()) {
        return filePath; // Возвращаем оригинальный путь если не удалось создать кэш
    }
    
    if (!fs::exists(filePath)) {
        return filePath;
    }
    
    std::string hash = urlToHash(url);
    std::string cacheDir = getCacheDir();
    std::string cachedPath = cacheDir + "/" + hash + ".mp4";
    
    try {
        fs::copy_file(filePath, cachedPath, fs::copy_options::overwrite_existing);
        return cachedPath;
    } catch (...) {
        return filePath; // Возвращаем оригинальный путь при ошибке
    }
}

void VideoCache::clearCache() {
    std::string cacheDir = getCacheDir();
    if (fs::exists(cacheDir) && fs::is_directory(cacheDir)) {
        try {
            for (const auto& entry : fs::directory_iterator(cacheDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".mp4") {
                    fs::remove(entry.path());
                }
            }
        } catch (...) {
            // Игнорируем ошибки при очистке
        }
    }
}

} // namespace video2ascii
