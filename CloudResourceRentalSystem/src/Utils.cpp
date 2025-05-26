#include "Utils.h"
#include <string>
#include <chrono>
#include <iomanip> // For std::put_time
#include <sstream> // For std::ostringstream
#include <ctime>   // For std::localtime

// Function to generate a unique ID with a given prefix and current size
std::string generateUniqueId(const std::string& prefix, int currentSize) {
    return prefix + std::to_string(currentSize + 1);
}

// Function to format a time_point to a string
std::string formatTimePoint(const std::chrono::system_clock::time_point& tp, const std::string& format) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    // Using std::localtime which is not thread-safe. For production, consider alternatives.
    // For Windows, localtime_s; for POSIX, localtime_r.
    // However, for this project, std::localtime is acceptable given the context.
    std::tm tm = *std::localtime(&tt); 
    std::ostringstream oss;
    oss << std::put_time(&tm, format.c_str());
    return oss.str();
}
