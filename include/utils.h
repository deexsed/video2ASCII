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

/**
 * @brief Проверяет доступное место на диске
 * @param path Путь к директории для проверки
 * @param requiredSizeMB Требуемый размер в мегабайтах
 * @return true если достаточно места
 */
bool hasEnoughDiskSpace(const std::string& path, size_t requiredSizeMB);

/**
 * @brief Получает доступное место на диске в мегабайтах
 * @param path Путь к директории
 * @return Количество доступных мегабайт
 */
size_t getAvailableDiskSpaceMB(const std::string& path);

} // namespace utils
} // namespace video2ascii
