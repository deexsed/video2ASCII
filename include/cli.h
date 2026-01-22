#pragma once

#include <string>
#include "config.h"
#include "logger.h"

namespace video2ascii {
namespace cli {

/**
 * @brief Структура для хранения параметров командной строки
 */
struct Arguments {
    std::string videoPath;
    int width = config::DEFAULT_WIDTH;
    bool showHelp = false;
    bool interactive = false;  // Интерактивный режим
    std::string asciiStyle = "basic";  // basic, detailed, simple, dense, custom
    std::string customAsciiChars = "";
    bool colored = false;
    config::OutputMode outputMode = config::OutputMode::CONSOLE;
    std::string outputFile = "";
    bool showStats = true;
    LogLevel logLevel = LogLevel::INFO;
    std::string logFile = "";
    bool useCache = true;
    
    // Проверка валидности
    bool isValid() const {
        return !videoPath.empty();
    }
};

/**
 * @brief Парсит аргументы командной строки
 * @param argc Количество аргументов
 * @param argv Массив аргументов
 * @return Структура с распарсенными аргументами
 */
Arguments parseArguments(int argc, char* argv[]);

/**
 * @brief Выводит справку по использованию программы
 * @param programName Имя программы
 */
void printUsage(const char* programName);

} // namespace cli
} // namespace video2ascii
