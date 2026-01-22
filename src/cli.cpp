#include "cli.h"
#include "config.h"
#include <iostream>
#include <stdexcept>

namespace video2ascii {
namespace cli {

namespace {
    LogLevel parseLogLevel(const std::string& level) {
        if (level == "debug") return LogLevel::DEBUG;
        if (level == "info") return LogLevel::INFO;
        if (level == "warning") return LogLevel::WARNING;
        if (level == "error") return LogLevel::ERROR;
        return LogLevel::INFO;
    }
}

void printUsage(const char* programName) {
    std::cout << "Использование: " << programName << " [опции] <путь_к_видео_или_URL>" << std::endl;
    std::cout << "\nОсновные опции:" << std::endl;
    std::cout << "  -w, --width <число>       Ширина ASCII-изображения (по умолчанию: " 
              << config::DEFAULT_WIDTH << ", мин: " << config::MIN_WIDTH 
              << ", макс: " << config::MAX_WIDTH << ")" << std::endl;
    std::cout << "  -h, --help                Показать эту справку" << std::endl;
    std::cout << "\nСтили ASCII:" << std::endl;
    std::cout << "  -s, --style <стиль>       Стиль ASCII (basic, detailed, simple, dense)" << std::endl;
    std::cout << "  -c, --chars <символы>     Пользовательский набор ASCII символов" << std::endl;
    std::cout << "  --colored                 Использовать цветной ASCII (ANSI цвета)" << std::endl;
    std::cout << "\nВывод:" << std::endl;
    std::cout << "  -o, --output <файл>       Сохранить ASCII в файл" << std::endl;
    std::cout << "  --both                    Выводить в консоль и файл" << std::endl;
    std::cout << "\nДополнительно:" << std::endl;
    std::cout << "  --no-stats                Не показывать статистику" << std::endl;
    std::cout << "  --log-level <уровень>     Уровень логирования (debug, info, warning, error)" << std::endl;
    std::cout << "  --log-file <файл>         Файл для логирования" << std::endl;
    std::cout << "  --no-cache                Не использовать кэш для загруженных видео" << std::endl;
    std::cout << "  -i, --interactive         Запустить интерактивный режим настройки" << std::endl;
    std::cout << "\nПримеры:" << std::endl;
    std::cout << "  " << programName << " video.mp4" << std::endl;
    std::cout << "  " << programName << " -w 300 --colored video.mp4" << std::endl;
    std::cout << "  " << programName << " -s detailed -o output.txt video.mp4" << std::endl;
    std::cout << "  " << programName << " --style dense --colored https://example.com/video.mp4" << std::endl;
}

Arguments parseArguments(int argc, char* argv[]) {
    Arguments args;
    
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
                    if (args.width < config::MIN_WIDTH || args.width > config::MAX_WIDTH) {
                        throw std::runtime_error("Ширина должна быть от " + 
                                                std::to_string(config::MIN_WIDTH) + 
                                                " до " + std::to_string(config::MAX_WIDTH));
                    }
                } catch (const std::exception& e) {
                    throw std::runtime_error("Ошибка: " + std::string(e.what()));
                }
            } else {
                throw std::runtime_error("Ошибка: требуется значение для -w");
            }
        } else if (arg == "-s" || arg == "--style") {
            if (i + 1 < argc) {
                args.asciiStyle = argv[++i];
            } else {
                throw std::runtime_error("Ошибка: требуется значение для -s");
            }
        } else if (arg == "-c" || arg == "--chars") {
            if (i + 1 < argc) {
                args.customAsciiChars = argv[++i];
            } else {
                throw std::runtime_error("Ошибка: требуется значение для -c");
            }
        } else if (arg == "--colored") {
            args.colored = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
                args.outputMode = config::OutputMode::FILE;
            } else {
                throw std::runtime_error("Ошибка: требуется значение для -o");
            }
        } else if (arg == "--both") {
            args.outputMode = config::OutputMode::BOTH;
        } else if (arg == "--no-stats") {
            args.showStats = false;
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                args.logLevel = parseLogLevel(argv[++i]);
            } else {
                throw std::runtime_error("Ошибка: требуется значение для --log-level");
            }
        } else if (arg == "--log-file") {
            if (i + 1 < argc) {
                args.logFile = argv[++i];
            } else {
                throw std::runtime_error("Ошибка: требуется значение для --log-file");
            }
        } else if (arg == "--no-cache") {
            args.useCache = false;
        } else if (arg == "-i" || arg == "--interactive") {
            args.interactive = true;
        } else if (arg[0] != '-') {
            args.videoPath = arg;
        } else {
            throw std::runtime_error("Неизвестная опция: " + arg);
        }
    }
    
    // Если интерактивный режим или нет аргументов, не требуем videoPath
    if (args.videoPath.empty() && !args.showHelp && !args.interactive && argc > 1) {
        throw std::runtime_error("Ошибка: не указан путь к видео или URL");
    }
    
    return args;
}

} // namespace cli
} // namespace video2ascii
