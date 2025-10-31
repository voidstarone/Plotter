#include "ConsoleLogger.h"
#include <iostream>

namespace plotter {
namespace logger {

void ConsoleLogger::debug(const std::string& message) {
    std::cout << "[DEBUG] " << message << std::endl;
}

void ConsoleLogger::info(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void ConsoleLogger::warning(const std::string& message) {
    std::cerr << "[WARNING] " << message << std::endl;
}

void ConsoleLogger::error(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void ConsoleLogger::log(LogLevel level, const std::string& message) {
    switch(level) {
        case LogLevel::DEBUG:
            debug(message);
            break;
        case LogLevel::INFO:
            info(message);
            break;
        case LogLevel::WARNING:
            warning(message);
            break;
        case LogLevel::ERROR:
            error(message);
            break;
    }
}

} // namespace logger
} // namespace plotter
