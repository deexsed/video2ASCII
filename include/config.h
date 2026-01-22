#pragma once

#include <string>
#include <vector>

namespace video2ascii {
namespace config {

// Набор символов для отрисовки ASCII-арта
constexpr const char* ASCII_CHARS = "    ._-^*+:=?&$@#%";
constexpr int ASCII_CHARS_LENGTH = 16;
constexpr int MAX_PIXEL_INTENSITY = 256;
constexpr int DEFAULT_WIDTH = 250;
constexpr double HEIGHT_RATIO = 0.5;  // Коэффициент высоты для лучшей читаемости
constexpr double DEFAULT_FPS = 30.0;  // FPS по умолчанию, если не удалось определить

// Ограничения
constexpr int MIN_WIDTH = 10;
constexpr int MAX_WIDTH = 1000;
constexpr size_t MAX_FRAMES_IN_MEMORY = 100;  // Максимальное количество кадров в буфере
constexpr size_t MIN_DISK_SPACE_MB = 100;     // Минимальное свободное место на диске (MB)

// Таймауты
constexpr int DOWNLOAD_TIMEOUT_SECONDS = 300;  // 5 минут

// ASCII стили
namespace ascii_styles {
    constexpr const char* BASIC = "    ._-^*+:=?&$@#%";
    constexpr const char* DETAILED = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    constexpr const char* SIMPLE = " .:-=+*#%@";
    constexpr const char* DENSE = "█▓▒░ ";
}

// Режимы вывода
enum class OutputMode {
    CONSOLE,
    FILE,
    BOTH
};

} // namespace config
} // namespace video2ascii
