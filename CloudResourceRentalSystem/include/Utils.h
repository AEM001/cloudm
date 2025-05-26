#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <chrono> // Required for std::chrono::system_clock::time_point

// Function to generate a unique ID with a given prefix and current size
std::string generateUniqueId(const std::string& prefix, int currentSize);

// Function to format a time_point to a string
std::string formatTimePoint(const std::chrono::system_clock::time_point& tp, const std::string& format = "%Y-%m-%d %H:%M:%S");

#endif // UTILS_H
