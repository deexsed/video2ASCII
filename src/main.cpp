#include "video_processor.h"
#include "video_path_resolver.h"
#include "cli.h"
#include "config.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    try {
        // Парсинг аргументов командной строки
        auto args = video2ascii::cli::parseArguments(argc, argv);
        
        if (args.showHelp) {
            video2ascii::cli::printUsage(argv[0]);
            return 0;
        }
        
        // Получаем путь к видео (загружаем если это URL)
        std::string finalPath = video2ascii::VideoPathResolver::resolve(args.videoPath);
        bool isTempFile = video2ascii::VideoPathResolver::isTemporaryFile(finalPath);
        
        try {
            // Создаем процессор видео (RAII)
            video2ascii::VideoProcessor processor(finalPath, args.width);
            
            std::cout << "Обработка видео..." << std::endl;
            std::cout << "Размер: " << processor.getWidth() << "x" << processor.getHeight() << std::endl;
            std::cout << "FPS: " << processor.getFps() << std::endl;
            std::cout << "Нажмите Ctrl+C для выхода" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            processor.process();
            
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
            if (isTempFile && fs::exists(finalPath)) {
                fs::remove(finalPath);
            }
            return 1;
        }
        
        // Удаляем временный файл если он был загружен
        if (isTempFile && fs::exists(finalPath)) {
            fs::remove(finalPath);
            std::cout << "\nВременный файл удален." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        video2ascii::cli::printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}
