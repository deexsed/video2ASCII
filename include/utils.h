#pragma once

#include <string>

namespace video2ascii {
namespace utils {

/**
 * @brief Очищает экран терминала (кроссплатформенно)
 */
void clearScreen();

/**
 * @brief Проверяет, является ли строка URL
 * @param path Путь или URL для проверки
 * @return true если это URL, false иначе
 */
bool isUrl(const std::string& path);

/**
 * @brief Генерирует уникальное имя временного файла
 * @param extension Расширение файла (например, ".mp4")
 * @return Уникальное имя файла
 */
std::string generateTempFileName(const std::string& extension = ".mp4");

} // namespace utils
} // namespace video2ascii
