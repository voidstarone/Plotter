#ifndef CONSOLE_LOGGER_H
#define CONSOLE_LOGGER_H

#include "Logger.h"
#include <iostream>

namespace plotter {
namespace logger {

/**
 * @brief Simple console-based logger implementation
 */
class ConsoleLogger : public Logger {
public:
    void debug(const std::string& message) override {
        std::cout << "[DEBUG] " << message << std::endl;
    }
    
    void info(const std::string& message) override {
        std::cout << "[INFO] " << message << std::endl;
    }
    
    void warning(const std::string& message) override {
        std::cerr << "[WARNING] " << message << std::endl;
    }
    
    void error(const std::string& message) override {
        std::cerr << "[ERROR] " << message << std::endl;
    }
    
    void log(LogLevel level, const std::string& message) override {
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
};

} // namespace logger
} // namespace plotter

#endif // CONSOLE_LOGGER_H
