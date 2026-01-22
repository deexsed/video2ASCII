#pragma once

#include "config.h"

namespace video2ascii {

/**
 * @brief Класс для кеширования преобразования пикселей в ASCII символы (RAII)
 * 
 * Предварительно вычисляет индексы ASCII символов для всех возможных
 * значений интенсивности пикселя (0-255) для оптимизации производительности.
 */
class AsciiCache {
public:
    /**
     * @brief Конструктор - инициализирует кеш
     */
    AsciiCache();
    
    /**
     * @brief Преобразует интенсивность пикселя в ASCII символ
     * @param pixel_intensity Интенсивность пикселя (0-255)
     * @return ASCII символ для данной интенсивности
     */
    char pixelToASCII(int pixel_intensity) const;

private:
    int cache_[config::MAX_PIXEL_INTENSITY];
};

} // namespace video2ascii
