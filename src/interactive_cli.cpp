#include "interactive_cli.h"
#include "config.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace video2ascii {
namespace interactive {

void InteractiveCLI::clearScreen() {
    utils::clearScreen();
}

void InteractiveCLI::printHeader() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║           video2ASCII - Интерактивная настройка          ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void InteractiveCLI::printSeparator() {
    std::cout << "─────────────────────────────────────────────────────────────\n";
}

void InteractiveCLI::waitForEnter() {
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
}

std::string InteractiveCLI::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

int InteractiveCLI::getNumber(const std::string& prompt, int min, int max, int defaultValue) {
    int value;
    std::string input;
    
    while (true) {
        std::cout << prompt << " [" << defaultValue << "]: ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            return defaultValue;
        }
        
        try {
            value = std::stoi(input);
            if (value >= min && value <= max) {
                return value;
            } else {
                std::cout << "Ошибка: значение должно быть от " << min << " до " << max << "\n";
            }
        } catch (...) {
            std::cout << "Ошибка: введите число\n";
        }
    }
}

int InteractiveCLI::showMenu(const std::string& title, const std::vector<std::string>& options) {
    clearScreen();
    printHeader();
    std::cout << title << "\n";
    printSeparator();
    
    for (size_t i = 0; i < options.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << options[i] << "\n";
    }
    std::cout << "  0. Назад\n";
    printSeparator();
    
    int choice = getNumber("Выберите опцию", 0, static_cast<int>(options.size()), 0);
    return choice;
}

std::string InteractiveCLI::selectVideoFile() {
    clearScreen();
    printHeader();
    std::cout << "Выбор видео файла\n";
    printSeparator();
    std::cout << "1. Ввести путь к локальному файлу\n";
    std::cout << "2. Ввести URL для загрузки\n";
    std::cout << "0. Назад\n";
    printSeparator();
    
    int choice = getNumber("Выберите опцию", 0, 2, 0);
    
    if (choice == 0) {
        return "";
    }
    
    std::string path;
    if (choice == 1) {
        path = getInput("Введите путь к видео файлу: ");
        
        // Проверка существования файла
        if (!path.empty() && !fs::exists(path)) {
            std::cout << "⚠ Предупреждение: файл не найден. Продолжить? (y/n): ";
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm != "y" && confirm != "Y") {
                return "";
            }
        }
    } else if (choice == 2) {
        path = getInput("Введите URL видео: ");
        
        if (!path.empty() && !utils::isUrl(path)) {
            std::cout << "⚠ Предупреждение: это не похоже на URL. Продолжить? (y/n): ";
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm != "y" && confirm != "Y") {
                return "";
            }
        }
    }
    
    return path;
}

int InteractiveCLI::configureWidth() {
    clearScreen();
    printHeader();
    std::cout << "Настройка ширины ASCII-изображения\n";
    printSeparator();
    std::cout << "Рекомендуемые значения:\n";
    std::cout << "  - Узкое (30-50): для маленьких терминалов\n";
    std::cout << "  - Среднее (50-100): стандартное использование\n";
    std::cout << "  - Широкое (100-200): для больших экранов\n";
    printSeparator();
    
    return getNumber("Введите ширину", config::MIN_WIDTH, config::MAX_WIDTH, config::DEFAULT_WIDTH);
}

std::string InteractiveCLI::selectAsciiStyle() {
    std::vector<std::string> styles = {
        "basic - Базовый набор (16 символов)",
        "detailed - Детализированный (70+ символов)",
        "simple - Простой (10 символов)",
        "dense - Плотный (Unicode символы)",
        "custom - Пользовательский набор"
    };
    
    int choice = showMenu("Выбор стиля ASCII", styles);
    
    if (choice == 0) {
        return "basic";
    }
    
    switch (choice) {
        case 1: return "basic";
        case 2: return "detailed";
        case 3: return "simple";
        case 4: return "dense";
        case 5: {
            std::string custom = getInput("Введите набор символов (от светлого к темному): ");
            return custom.empty() ? "basic" : custom;
        }
        default: return "basic";
    }
}

bool InteractiveCLI::configureColored() {
    std::vector<std::string> options = {
        "Нет - Черно-белый ASCII",
        "Да - Цветной ASCII (ANSI цвета)"
    };
    
    int choice = showMenu("Использовать цветной ASCII?", options);
    return choice == 2;
}

config::OutputMode InteractiveCLI::configureOutput() {
    std::vector<std::string> options = {
        "Консоль - Вывод только в терминал",
        "Файл - Сохранение только в файл",
        "Оба - Консоль и файл одновременно"
    };
    
    int choice = showMenu("Режим вывода", options);
    
    switch (choice) {
        case 1: return config::OutputMode::CONSOLE;
        case 2: return config::OutputMode::FILE;
        case 3: return config::OutputMode::BOTH;
        default: return config::OutputMode::CONSOLE;
    }
}

std::string InteractiveCLI::configureOutputFile() {
    clearScreen();
    printHeader();
    std::cout << "Настройка файла вывода\n";
    printSeparator();
    
    std::string file = getInput("Введите путь к файлу [output_ascii.txt]: ");
    return file.empty() ? "output_ascii.txt" : file;
}

void InteractiveCLI::configureLogging(cli::Arguments& args) {
    std::vector<std::string> levels = {
        "debug - Подробная отладочная информация",
        "info - Общая информация",
        "warning - Только предупреждения",
        "error - Только ошибки"
    };
    
    int choice = showMenu("Уровень логирования", levels);
    
    switch (choice) {
        case 1: args.logLevel = LogLevel::DEBUG; break;
        case 2: args.logLevel = LogLevel::INFO; break;
        case 3: args.logLevel = LogLevel::WARNING; break;
        case 4: args.logLevel = LogLevel::ERROR; break;
        default: args.logLevel = LogLevel::INFO; break;
    }
    
    std::string logFile = getInput("Путь к файлу логов (Enter для отключения): ");
    if (!logFile.empty()) {
        args.logFile = logFile;
    }
}

void InteractiveCLI::configureAdvanced(cli::Arguments& args) {
    while (true) {
        std::vector<std::string> options;
        options.push_back("Показывать статистику: " + std::string(args.showStats ? "Да" : "Нет"));
        options.push_back("Использовать кэш: " + std::string(args.useCache ? "Да" : "Нет"));
        
        int choice = showMenu("Дополнительные настройки", options);
        
        if (choice == 0) {
            break;
        }
        
        switch (choice) {
            case 1:
                args.showStats = !args.showStats;
                break;
            case 2:
                args.useCache = !args.useCache;
                break;
        }
    }
}

void InteractiveCLI::showPreview(const cli::Arguments& args) {
    clearScreen();
    printHeader();
    std::cout << "Предпросмотр настроек\n";
    printSeparator();
    
    std::cout << "Видео: " << (args.videoPath.empty() ? "(не выбрано)" : args.videoPath) << "\n";
    std::cout << "Ширина: " << args.width << "\n";
    std::cout << "Стиль ASCII: " << args.asciiStyle;
    if (!args.customAsciiChars.empty()) {
        std::cout << " (custom: " << args.customAsciiChars << ")";
    }
    std::cout << "\n";
    std::cout << "Цветной режим: " << (args.colored ? "Да" : "Нет") << "\n";
    
    std::string outputMode;
    switch (args.outputMode) {
        case config::OutputMode::CONSOLE: outputMode = "Консоль"; break;
        case config::OutputMode::FILE: outputMode = "Файл"; break;
        case config::OutputMode::BOTH: outputMode = "Консоль + Файл"; break;
    }
    std::cout << "Режим вывода: " << outputMode << "\n";
    
    if (args.outputMode != config::OutputMode::CONSOLE) {
        std::cout << "Файл вывода: " << (args.outputFile.empty() ? "output_ascii.txt" : args.outputFile) << "\n";
    }
    
    std::string logLevel;
    switch (args.logLevel) {
        case LogLevel::DEBUG: logLevel = "DEBUG"; break;
        case LogLevel::INFO: logLevel = "INFO"; break;
        case LogLevel::WARNING: logLevel = "WARNING"; break;
        case LogLevel::ERROR: logLevel = "ERROR"; break;
    }
    std::cout << "Уровень логирования: " << logLevel << "\n";
    
    if (!args.logFile.empty()) {
        std::cout << "Файл логов: " << args.logFile << "\n";
    }
    
    std::cout << "Статистика: " << (args.showStats ? "Да" : "Нет") << "\n";
    std::cout << "Кэш: " << (args.useCache ? "Да" : "Нет") << "\n";
    
    printSeparator();
    waitForEnter();
}

cli::Arguments InteractiveCLI::run() {
    cli::Arguments args;
    
    while (true) {
        clearScreen();
        printHeader();
        std::cout << "Главное меню\n";
        printSeparator();
        
        std::vector<std::string> menuItems;
        menuItems.push_back("Выбрать видео файл" + std::string(args.videoPath.empty() ? " ⚠" : " ✓"));
        menuItems.push_back("Настроить ширину (" + std::to_string(args.width) + ")");
        menuItems.push_back("Выбрать стиль ASCII (" + args.asciiStyle + ")");
        menuItems.push_back("Настроить цвет (" + std::string(args.colored ? "Вкл" : "Выкл") + ")");
        menuItems.push_back("Настроить вывод");
        menuItems.push_back("Настроить логирование");
        menuItems.push_back("Дополнительные настройки");
        menuItems.push_back("Предпросмотр настроек");
        menuItems.push_back("▶ Запустить обработку");
        menuItems.push_back("❌ Выход");
        
        for (size_t i = 0; i < menuItems.size(); i++) {
            std::cout << "  " << (i + 1) << ". " << menuItems[i] << "\n";
        }
        std::cout << "  0. Выход без запуска\n";
        printSeparator();
        
        int choice = getNumber("Выберите опцию", 0, static_cast<int>(menuItems.size()), 0);
        
        if (choice == 0 || choice == 10) {
            return cli::Arguments(); // Пустые аргументы = отмена
        }
        
        switch (choice) {
            case 1: {
                std::string video = selectVideoFile();
                if (!video.empty()) {
                    args.videoPath = video;
                }
                break;
            }
            case 2:
                args.width = configureWidth();
                break;
            case 3: {
                std::string style = selectAsciiStyle();
                if (style == "basic" || style == "detailed" || style == "simple" || style == "dense") {
                    args.asciiStyle = style;
                    args.customAsciiChars.clear();
                } else {
                    // Пользовательские символы
                    args.asciiStyle = "custom";
                    args.customAsciiChars = style;
                }
                break;
            }
            case 4:
                args.colored = configureColored();
                break;
            case 5: {
                config::OutputMode mode = configureOutput();
                args.outputMode = mode;
                if (mode != config::OutputMode::CONSOLE) {
                    args.outputFile = configureOutputFile();
                }
                break;
            }
            case 6:
                configureLogging(args);
                break;
            case 7:
                configureAdvanced(args);
                break;
            case 8:
                showPreview(args);
                break;
            case 9: {
                if (args.videoPath.empty()) {
                    std::cout << "\n⚠ Ошибка: не выбран видео файл!\n";
                    waitForEnter();
                    break;
                }
                
                std::cout << "\n▶ Запуск обработки...\n";
                waitForEnter();
                return args;
            }
        }
    }
}

} // namespace interactive
} // namespace video2ascii
