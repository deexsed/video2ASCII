#include "utils.h"
#include <cstdlib>
#include <chrono>
#include <sstream>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #define CLEAR_SCREEN_CMD "cls"
#else
    #include <unistd.h>
    #include <sys/statvfs.h>
    #define CLEAR_SCREEN_CMD "clear"
#endif

namespace fs = std::filesystem;

namespace video2ascii {
namespace utils {

void clearScreen() {
    int result = system(CLEAR_SCREEN_CMD);
    (void)result; // Игнорируем возвращаемое значение
}

bool isUrl(const std::string& path) {
    return path.find("http://") == 0 || path.find("https://") == 0;
}

std::string generateTempFileName(const std::string& extension) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::ostringstream oss;
    oss << "temp_video_" << timestamp << extension;
    return oss.str();
}

size_t getAvailableDiskSpaceMB(const std::string& path) {
#ifdef _WIN32
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalBytes;
    ULARGE_INTEGER totalFreeBytes;
    
    std::wstring wpath(path.begin(), path.end());
    if (GetDiskFreeSpaceExW(wpath.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        return static_cast<size_t>(freeBytesAvailable.QuadPart / (1024 * 1024));
    }
    return 0;
#else
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) == 0) {
        uint64_t freeBytes = static_cast<uint64_t>(stat.f_bavail) * stat.f_frsize;
        return static_cast<size_t>(freeBytes / (1024 * 1024));
    }
    return 0;
#endif
}

bool hasEnoughDiskSpace(const std::string& path, size_t requiredSizeMB) {
    size_t availableMB = getAvailableDiskSpaceMB(path);
    return availableMB >= requiredSizeMB;
}

} // namespace utils
} // namespace video2ascii
