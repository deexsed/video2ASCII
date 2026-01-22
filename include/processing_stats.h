#pragma once

#include <cstddef>
#include <chrono>
#include <atomic>

namespace video2ascii {

/**
 * @brief Статистика обработки видео
 */
struct ProcessingStats {
    std::atomic<size_t> framesProcessed{0};
    std::atomic<size_t> framesRendered{0};
    std::atomic<size_t> framesSkipped{0};
    std::atomic<size_t> memoryUsedMB{0};
    
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    double getProcessingFps() const;
    double getRenderingFps() const;
    double getElapsedSeconds() const;
    size_t getTotalFrames() const;
    
    void reset();
    void print() const;
};

} // namespace video2ascii
