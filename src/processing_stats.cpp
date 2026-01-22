#include "processing_stats.h"
#include <iostream>
#include <iomanip>
#include <chrono>

namespace video2ascii {

double ProcessingStats::getProcessingFps() const {
    double elapsed = getElapsedSeconds();
    if (elapsed <= 0) return 0.0;
    return framesProcessed.load() / elapsed;
}

double ProcessingStats::getRenderingFps() const {
    double elapsed = getElapsedSeconds();
    if (elapsed <= 0) return 0.0;
    return framesRendered.load() / elapsed;
}

double ProcessingStats::getElapsedSeconds() const {
    auto end = (endTime.time_since_epoch().count() == 0) ? 
               std::chrono::steady_clock::now() : endTime;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - startTime);
    return duration.count() / 1000.0;
}

size_t ProcessingStats::getTotalFrames() const {
    return framesProcessed.load() + framesSkipped.load();
}

void ProcessingStats::reset() {
    framesProcessed = 0;
    framesRendered = 0;
    framesSkipped = 0;
    memoryUsedMB = 0;
    startTime = std::chrono::steady_clock::now();
    endTime = std::chrono::steady_clock::time_point{};
}

void ProcessingStats::print() const {
    std::cout << "\n=== Статистика обработки ===" << std::endl;
    std::cout << "Обработано кадров: " << framesProcessed.load() << std::endl;
    std::cout << "Воспроизведено кадров: " << framesRendered.load() << std::endl;
    std::cout << "Пропущено кадров: " << framesSkipped.load() << std::endl;
    std::cout << "FPS обработки: " << std::fixed << std::setprecision(2) 
              << getProcessingFps() << std::endl;
    std::cout << "FPS воспроизведения: " << std::fixed << std::setprecision(2) 
              << getRenderingFps() << std::endl;
    std::cout << "Время обработки: " << std::fixed << std::setprecision(2) 
              << getElapsedSeconds() << " сек" << std::endl;
    std::cout << "Использовано памяти: " << memoryUsedMB.load() << " MB" << std::endl;
    std::cout << "=============================" << std::endl;
}

} // namespace video2ascii
