#include "video_downloader.h"
#include "utils.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>

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

std::string VideoDownloader::buildCurlCommand(const std::string& url, const std::string& outputPath) {
    std::string command;
#ifdef _WIN32
    command = "curl.exe -L -o \"" + outputPath + "\" \"" + url + "\"";
#else
    command = "curl -L -o \"" + outputPath + "\" \"" + url + "\"";
#endif
    return command;
}

bool VideoDownloader::downloadFromUrl(const std::string& url, const std::string& outputPath) {
    std::cout << "Загрузка видео по ссылке: " << url << std::endl;
    
    if (!isCurlAvailable()) {
        std::cerr << "Ошибка: curl не найден. Установите curl для загрузки видео." << std::endl;
        return false;
    }
    
    std::string command = buildCurlCommand(url, outputPath);
    int result = system(command.c_str());
    
    if (result != 0) {
        std::cerr << "Ошибка при загрузке видео." << std::endl;
        return false;
    }
    
    if (!fs::exists(outputPath) || fs::file_size(outputPath) == 0) {
        std::cerr << "Ошибка: загруженный файл пуст или не существует." << std::endl;
        return false;
    }
    
    std::cout << "Видео успешно загружено: " << outputPath << std::endl;
    return true;
}

} // namespace video2ascii
