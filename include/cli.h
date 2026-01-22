#pragma once

#include <string>
#include "config.h"

namespace video2ascii {
namespace cli {

/**
 * @brief Структура для хранения параметров командной строки
 */
struct Arguments {
    std::string videoPath;
    int width = config::DEFAULT_WIDTH;
    bool showHelp = false;
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
