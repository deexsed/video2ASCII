#include "video_downloader.h"
#include "utils.h"
#include "logger.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <cmath>

namespace fs = std::filesystem;

namespace video2ascii {

bool VideoDownloader::isCurlAvailable() {
    int curlCheck = system("curl --version > /dev/null 2>&1");
    if (curlCheck != 0) {
#ifdef _WIN32
        curlCheck = system("curl.exe --version >nul 2>&1");
#endif
        return curlCheck == 0;
    }
    return true;
}

std::string VideoDownloader::buildCurlCommand(const std::string& url, 
                                             const std::string& outputPath,
                                             int timeoutSeconds,
                                             bool showProgress) {
    std::string command;
#ifdef _WIN32
    command = "curl.exe";
#else
    command = "curl";
#endif
    
    command += " -L"; // Следовать редиректам
    
    if (timeoutSeconds > 0) {
        command += " --max-time " + std::to_string(timeoutSeconds);
    }
    
    if (showProgress) {
        command += " --progress-bar";
    } else {
        command += " -s"; // Тихий режим
    }
    
    command += " -o \"" + outputPath + "\"";
    command += " \"" + url + "\"";
    
    return command;
}

void VideoDownloader::showProgressBar(size_t downloaded, size_t total) {
    if (total == 0) {
        std::cout << "\rЗагружено: " << (downloaded / 1024 / 1024) << " MB     ";
        std::cout.flush();
        return;
    }
    
    const int barWidth = 50;
    float progress = static_cast<float>(downloaded) / total;
    int pos = static_cast<int>(barWidth * progress);
    
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "% ";
    std::cout << "(" << (downloaded / 1024 / 1024) << " / " << (total / 1024 / 1024) << " MB)";
    std::cout.flush();
}

bool VideoDownloader::downloadFromUrl(const std::string& url, 
                                     const std::string& outputPath,
                                     int timeoutSeconds,
                                     ProgressCallback progressCallback) {
    LOG_INFO("Загрузка видео по ссылке: " + url);
    
    if (!isCurlAvailable()) {
        LOG_ERROR("curl не найден. Установите curl для загрузки видео.");
        return false;
    }
    
    // Проверяем место на диске перед загрузкой
    size_t estimatedSizeMB = 500; // Примерная оценка
    std::string dir = fs::path(outputPath).parent_path().string();
    if (dir.empty()) dir = ".";
    
    if (!utils::hasEnoughDiskSpace(dir, estimatedSizeMB)) {
        LOG_ERROR("Недостаточно места на диске для загрузки видео");
        return false;
    }
    
    bool showProgress = (progressCallback != nullptr);
    std::string command = buildCurlCommand(url, outputPath, timeoutSeconds, showProgress);
    
    if (showProgress && progressCallback) {
        std::cout << "Загрузка..." << std::endl;
    }
    
    int result = system(command.c_str());
    
    if (result != 0) {
        LOG_ERROR("Ошибка при загрузке видео (код: " + std::to_string(result) + ")");
        return false;
    }
    
    if (!fs::exists(outputPath) || fs::file_size(outputPath) == 0) {
        LOG_ERROR("Загруженный файл пуст или не существует");
        return false;
    }
    
    size_t fileSize = fs::file_size(outputPath);
    if (progressCallback) {
        progressCallback(fileSize, fileSize);
        std::cout << std::endl;
    }
    
    LOG_INFO("Видео успешно загружено: " + outputPath + " (" + 
             std::to_string(fileSize / 1024 / 1024) + " MB)");
    return true;
}

} // namespace video2ascii
