#include "ascii_cache.h"

namespace video2ascii {

AsciiCache::AsciiCache() {
    for (int i = 0; i < config::MAX_PIXEL_INTENSITY; i++) {
        cache_[i] = (i * config::ASCII_CHARS_LENGTH) / config::MAX_PIXEL_INTENSITY;
    }
}

char AsciiCache::pixelToASCII(int pixel_intensity) const {
    if (pixel_intensity < 0 || pixel_intensity >= config::MAX_PIXEL_INTENSITY) {
        return config::ASCII_CHARS[0];
    }
    return config::ASCII_CHARS[cache_[pixel_intensity]];
}

} // namespace video2ascii
