#pragma once

#include <string>
#include <vector>

namespace video2ascii {

/**
 * @brief Класс для валидации видео файлов
 */
class VideoValidator {
public:
    /**
     * @brief Проверяет, является ли файл валидным видео
     * @param filepath Путь к файлу
     * @return true если файл валидный
     */
    static bool isValidVideoFile(const std::string& filepath);
    
    /**
     * @brief Проверяет расширение файла
     * @param filepath Путь к файлу
     * @return true если расширение поддерживается
     */
    static bool hasValidExtension(const std::string& filepath);
    
    /**
     * @brief Получить список поддерживаемых расширений
     */
    static std::vector<std::string> getSupportedExtensions();
    
    /**
     * @brief Проверяет MIME-тип файла (если доступно)
     * @param filepath Путь к файлу
     * @return true если MIME-тип валидный
     */
    static bool hasValidMimeType(const std::string& filepath);

private:
    static const std::vector<std::string> supportedExtensions_;
    static std::string getFileExtension(const std::string& filepath);
};

} // namespace video2ascii
