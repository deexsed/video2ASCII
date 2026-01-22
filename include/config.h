#pragma once

#include <string>

namespace video2ascii {
namespace config {

// Набор символов для отрисовки ASCII-арта
constexpr const char* ASCII_CHARS = "    ._-^*+:=?&$@#%";
constexpr int ASCII_CHARS_LENGTH = 16;
constexpr int MAX_PIXEL_INTENSITY = 256;
constexpr int DEFAULT_WIDTH = 250;
constexpr double HEIGHT_RATIO = 0.5;  // Коэффициент высоты для лучшей читаемости
constexpr double DEFAULT_FPS = 30.0;  // FPS по умолчанию, если не удалось определить

} // namespace config
} // namespace video2ascii
