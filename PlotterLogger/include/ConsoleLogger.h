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
    void debug(const std::string& message) override;
    void info(const std::string& message) override;
    void warning(const std::string& message) override;
    void error(const std::string& message) override;
    void log(LogLevel level, const std::string& message) override;
};

} // namespace logger
} // namespace plotter

#endif // CONSOLE_LOGGER_H
