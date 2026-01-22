#include "video_processor.h"
#include "video_path_resolver.h"
#include "cli.h"
#include "config.h"
#include "logger.h"
#include "signal_handler.h"
#include "video_cache.h"
#include "interactive_cli.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

std::string getAsciiChars(const video2ascii::cli::Arguments& args) {
    if (!args.customAsciiChars.empty()) {
        return args.customAsciiChars;
    }
    
    if (args.asciiStyle == "basic") return video2ascii::config::ascii_styles::BASIC;
    if (args.asciiStyle == "detailed") return video2ascii::config::ascii_styles::DETAILED;
    if (args.asciiStyle == "simple") return video2ascii::config::ascii_styles::SIMPLE;
    if (args.asciiStyle == "dense") return video2ascii::config::ascii_styles::DENSE;
    
    return video2ascii::config::ascii_styles::BASIC;
}

int main(int argc, char* argv[]) {
    // Инициализация обработчика сигналов
    video2ascii::SignalHandler::initialize();
    
    try {
        video2ascii::cli::Arguments args;
        
        // Если запущено без аргументов - интерактивный режим
        if (argc == 1) {
            args = video2ascii::interactive::InteractiveCLI::run();
            
            // Если пользователь отменил или не выбрал видео
            if (!args.isValid()) {
                std::cout << "Работа отменена.\n";
                return 0;
            }
        } else {
            // Парсинг аргументов командной строки
            args = video2ascii::cli::parseArguments(argc, argv);
            
            if (args.showHelp) {
                video2ascii::cli::printUsage(argv[0]);
                return 0;
            }
            
            // Интерактивный режим по флагу
            if (args.interactive) {
                args = video2ascii::interactive::InteractiveCLI::run();
                
                // Если пользователь отменил или не выбрал видео
                if (!args.isValid()) {
                    std::cout << "Работа отменена.\n";
                    return 0;
                }
            }
        }
        
        // Настройка логирования
        video2ascii::Logger::getInstance().setLevel(args.logLevel);
        if (!args.logFile.empty()) {
            video2ascii::Logger::getInstance().setFileOutput(args.logFile, true);
        }
        
        LOG_INFO("Запуск video2ASCII");
        LOG_DEBUG("Аргументы: width=" + std::to_string(args.width) + 
                 ", style=" + args.asciiStyle + 
                 ", colored=" + (args.colored ? "true" : "false"));
        
        // Получаем путь к видео (загружаем если это URL)
        std::string finalPath;
        bool isTempFile = false;
        
        try {
            // Проверяем кэш если это URL и кэш включен
            if (args.useCache && video2ascii::utils::isUrl(args.videoPath)) {
                std::string cachedPath = video2ascii::VideoCache::getCachedPath(args.videoPath);
                if (!cachedPath.empty()) {
                    LOG_INFO("Использование кэшированного видео");
                    finalPath = cachedPath;
                } else {
                    finalPath = video2ascii::VideoPathResolver::resolve(args.videoPath);
                    isTempFile = video2ascii::VideoPathResolver::isTemporaryFile(finalPath);
                    
                    // Сохраняем в кэш если это временный файл
                    if (isTempFile) {
                        video2ascii::VideoCache::cacheFile(args.videoPath, finalPath);
                    }
                }
            } else {
                finalPath = video2ascii::VideoPathResolver::resolve(args.videoPath);
                isTempFile = video2ascii::VideoPathResolver::isTemporaryFile(finalPath);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Ошибка при разрешении пути к видео: " + std::string(e.what()));
            throw;
        }
        
        // Получаем набор ASCII символов
        std::string asciiChars = getAsciiChars(args);
        
        try {
            // Создаем процессор видео (RAII)
            video2ascii::VideoProcessor processor(
                finalPath, 
                args.width,
                asciiChars,
                args.outputMode,
                args.outputFile,
                args.colored
            );
            
            std::cout << "Обработка видео..." << std::endl;
            std::cout << "Размер: " << processor.getWidth() << "x" << processor.getHeight() << std::endl;
            std::cout << "FPS: " << processor.getFps() << std::endl;
            std::cout << "Стиль: " << args.asciiStyle << std::endl;
            if (args.colored) {
                std::cout << "Цветной режим: включен" << std::endl;
            }
            std::cout << "Нажмите Ctrl+C для выхода" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            processor.process();
            
            // Выводим статистику если нужно
            if (args.showStats) {
                const auto& stats = processor.getStats();
                stats.print();
            }
            
        } catch (const std::exception& e) {
            LOG_ERROR("Ошибка обработки видео: " + std::string(e.what()));
            std::cerr << "Ошибка: " << e.what() << std::endl;
            if (isTempFile && fs::exists(finalPath)) {
                try {
                    fs::remove(finalPath);
                    LOG_DEBUG("Временный файл удален");
                } catch (...) {
                    LOG_WARNING("Не удалось удалить временный файл");
                }
            }
            return 1;
        }
        
        // Удаляем временный файл если он был загружен и не в кэше
        if (isTempFile && fs::exists(finalPath) && !args.useCache) {
            try {
                fs::remove(finalPath);
                std::cout << "\nВременный файл удален." << std::endl;
                LOG_DEBUG("Временный файл удален");
            } catch (...) {
                LOG_WARNING("Не удалось удалить временный файл");
            }
        }
        
        LOG_INFO("Завершение работы");
        
    } catch (const std::exception& e) {
        LOG_ERROR("Критическая ошибка: " + std::string(e.what()));
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        video2ascii::cli::printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}
