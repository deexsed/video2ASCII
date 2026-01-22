#include "ascii_cache.h"
#include <algorithm>

namespace video2ascii {

AsciiCache::AsciiCache(const std::string& asciiChars) 
    : asciiChars_(asciiChars.empty() ? config::ASCII_CHARS : asciiChars) {
    charsLength_ = static_cast<int>(asciiChars_.length());
    if (charsLength_ == 0) {
        asciiChars_ = config::ASCII_CHARS;
        charsLength_ = config::ASCII_CHARS_LENGTH;
    }
    initializeCache();
}

void AsciiCache::initializeCache() {
    for (int i = 0; i < config::MAX_PIXEL_INTENSITY; i++) {
        cache_[i] = (i * charsLength_) / config::MAX_PIXEL_INTENSITY;
        cache_[i] = std::min(cache_[i], charsLength_ - 1);
    }
}

void AsciiCache::setChars(const std::string& asciiChars) {
    if (asciiChars.empty()) {
        return;
    }
    asciiChars_ = asciiChars;
    charsLength_ = static_cast<int>(asciiChars_.length());
    initializeCache();
}

char AsciiCache::pixelToASCII(int pixel_intensity) const {
    if (pixel_intensity < 0 || pixel_intensity >= config::MAX_PIXEL_INTENSITY) {
        return asciiChars_[0];
    }
    int index = cache_[pixel_intensity];
    if (index < 0 || index >= charsLength_) {
        return asciiChars_[0];
    }
    return asciiChars_[index];
}

} // namespace video2ascii
