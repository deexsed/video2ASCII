#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>

namespace video2ascii {

/**
 * @brief Уровни логирования
 */
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

/**
 * @brief Класс для логирования (Singleton)
 */
class Logger {
public:
    /**
     * @brief Получить экземпляр логгера
     */
    static Logger& getInstance();
    
    /**
     * @brief Установить уровень логирования
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Включить/выключить вывод в файл
     */
    void setFileOutput(const std::string& filepath, bool enable);
    
    /**
     * @brief Логирование сообщения
     */
    void log(LogLevel level, const std::string& message);
    
    // Удобные методы
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    
    // Удаляем копирование
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    ~Logger();
    
    std::string levelToString(LogLevel level) const;
    std::string getCurrentTime() const;
    
    LogLevel currentLevel_ = LogLevel::INFO;
    bool fileOutputEnabled_ = false;
    std::ofstream logFile_;
    std::mutex mutex_;
};

// Макросы для удобства
#define LOG_DEBUG(msg) video2ascii::Logger::getInstance().debug(msg)
#define LOG_INFO(msg) video2ascii::Logger::getInstance().info(msg)
#define LOG_WARNING(msg) video2ascii::Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) video2ascii::Logger::getInstance().error(msg)

} // namespace video2ascii
