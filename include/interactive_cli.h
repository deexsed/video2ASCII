#pragma once

#include "cli.h"
#include <string>
#include <vector>

namespace video2ascii {
namespace interactive {

/**
 * @brief Интерактивный CLI интерфейс
 */
class InteractiveCLI {
public:
    /**
     * @brief Запустить интерактивный режим
     * @return Структура с настройками или пустая если отменено
     */
    static cli::Arguments run();
    
    /**
     * @brief Показать главное меню
     */
    static void showMainMenu();
    
    /**
     * @brief Показать меню выбора видео
     */
    static std::string selectVideoFile();
    
    /**
     * @brief Показать меню настройки ширины
     */
    static int configureWidth();
    
    /**
     * @brief Показать меню выбора стиля ASCII
     */
    static std::string selectAsciiStyle();
    
    /**
     * @brief Показать меню настройки цвета
     */
    static bool configureColored();
    
    /**
     * @brief Показать меню настройки вывода
     */
    static config::OutputMode configureOutput();
    
    /**
     * @brief Показать меню настройки файла вывода
     */
    static std::string configureOutputFile();
    
    /**
     * @brief Показать меню настройки логирования
     */
    static void configureLogging(cli::Arguments& args);
    
    /**
     * @brief Показать меню дополнительных настроек
     */
    static void configureAdvanced(cli::Arguments& args);
    
    /**
     * @brief Показать предпросмотр настроек
     */
    static void showPreview(const cli::Arguments& args);
    
    /**
     * @brief Очистить экран
     */
    static void clearScreen();
    
    /**
     * @brief Получить ввод от пользователя
     */
    static std::string getInput(const std::string& prompt);
    
    /**
     * @brief Получить число от пользователя
     */
    static int getNumber(const std::string& prompt, int min, int max, int defaultValue);
    
    /**
     * @brief Показать меню выбора
     */
    static int showMenu(const std::string& title, const std::vector<std::string>& options);

private:
    static void printHeader();
    static void printSeparator();
    static void waitForEnter();
};

} // namespace interactive
} // namespace video2ascii
