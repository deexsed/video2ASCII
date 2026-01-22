#pragma once

#include <atomic>
#include <csignal>

namespace video2ascii {

/**
 * @brief Класс для обработки сигналов (Ctrl+C, SIGTERM)
 */
class SignalHandler {
public:
    /**
     * @brief Инициализация обработчика сигналов
     */
    static void initialize();
    
    /**
     * @brief Проверить, был ли запрошен выход
     */
    static bool shouldExit();
    
    /**
     * @brief Сбросить флаг выхода
     */
    static void reset();
    
    /**
     * @brief Установить флаг выхода
     */
    static void requestExit();

private:
    static void handleSignal(int signal);
    static std::atomic<bool> exitRequested_;
};

} // namespace video2ascii
