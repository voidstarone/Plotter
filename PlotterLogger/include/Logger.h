#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace plotter {
namespace logger {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

/**
 * @brief Interface for logging throughout the application
 * 
 * This interface allows different logging implementations (console, file, etc.)
 * to be injected throughout the system without coupling to specific implementations.
 */
class Logger {
public:
    virtual ~Logger() = default;
    
    virtual void debug(const std::string& message) = 0;
    virtual void info(const std::string& message) = 0;
    virtual void warning(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
    
    virtual void log(LogLevel level, const std::string& message) = 0;
};

} // namespace logger
} // namespace plotter

#endif // LOGGER_H
