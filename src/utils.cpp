#include "utils.h"
#include <cstdlib>
#include <chrono>
#include <sstream>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN_CMD "cls"
#else
    #include <unistd.h>
    #define CLEAR_SCREEN_CMD "clear"
#endif

namespace video2ascii {
namespace utils {

void clearScreen() {
    system(CLEAR_SCREEN_CMD);
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

} // namespace utils
} // namespace video2ascii
