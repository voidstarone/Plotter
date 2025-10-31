# PlotterLogger

A simple header-only logger interface for the Plotter application.

## Overview

PlotterLogger provides a lightweight logging interface that can be injected throughout the application stack, from use cases down to data sources. This allows different logging implementations (console, file, etc.) without coupling to specific implementations.

## Interface

```cpp
namespace plotter::logger {
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };
    
    class Logger {
        virtual void debug(const std::string& message) = 0;
        virtual void info(const std::string& message) = 0;
        virtual void warning(const std::string& message) = 0;
        virtual void error(const std::string& message) = 0;
        virtual void log(LogLevel level, const std::string& message) = 0;
    };
}
```

## Usage

1. Implement the `Logger` interface with your preferred logging mechanism
2. Pass the logger instance through constructors from top to bottom
3. Call logging methods as needed

Example implementation:

```cpp
class ConsoleLogger : public plotter::logger::Logger {
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
            case LogLevel::DEBUG: debug(message); break;
            case LogLevel::INFO: info(message); break;
            case LogLevel::WARNING: warning(message); break;
            case LogLevel::ERROR: error(message); break;
        }
    }
};
```

## Integration

This is a header-only library. Link it in your CMakeLists.txt:

```cmake
target_link_libraries(YourTarget PRIVATE PlotterLogger)
```
