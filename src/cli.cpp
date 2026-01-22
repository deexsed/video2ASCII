#include "cli.h"
#include "config.h"
#include <iostream>
#include <stdexcept>

namespace video2ascii {
namespace cli {

void printUsage(const char* programName) {
    std::cout << "Использование: " << programName << " [опции] <путь_к_видео_или_URL>" << std::endl;
    std::cout << "Опции:" << std::endl;
    std::cout << "  -w, --width <число>    Ширина ASCII-изображения (по умолчанию: " 
              << config::DEFAULT_WIDTH << ")" << std::endl;
    std::cout << "  -h, --help             Показать эту справку" << std::endl;
    std::cout << std::endl;
    std::cout << "Примеры:" << std::endl;
    std::cout << "  " << programName << " video.mp4" << std::endl;
    std::cout << "  " << programName << " https://example.com/video.mp4" << std::endl;
    std::cout << "  " << programName << " -w 300 video.mp4" << std::endl;
}

Arguments parseArguments(int argc, char* argv[]) {
    Arguments args;
    args.width = config::DEFAULT_WIDTH;
    
    if (argc < 2) {
        args.showHelp = true;
        return args;
    }
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            args.showHelp = true;
            return args;
        } else if (arg == "-w" || arg == "--width") {
            if (i + 1 < argc) {
                try {
                    args.width = std::stoi(argv[++i]);
                    if (args.width <= 0) {
                        throw std::invalid_argument("Ширина должна быть положительным числом");
                    }
                } catch (const std::exception& e) {
                    throw std::runtime_error("Ошибка: ширина должна быть положительным числом");
                }
            } else {
                throw std::runtime_error("Ошибка: требуется значение для -w");
            }
        } else if (arg[0] != '-') {
            args.videoPath = arg;
        } else {
            throw std::runtime_error("Неизвестная опция: " + arg);
        }
    }
    
    if (args.videoPath.empty() && !args.showHelp) {
        throw std::runtime_error("Ошибка: не указан путь к видео или URL");
    }
    
    return args;
}

} // namespace cli
} // namespace video2ascii
