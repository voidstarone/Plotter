#ifndef USECASECOMMON_H
#define USECASECOMMON_H

#include <chrono>
#include <string>
#include <functional>

namespace UseCase {

/**
 * @brief Error categories for use case operations
 */
enum class ErrorCategory {
    VALIDATION_ERROR,      // Input validation failed
    TIMEOUT_ERROR,         // Operation timed out
    REPOSITORY_ERROR,      // Repository/persistence failure
    BUSINESS_RULE_ERROR,   // Business logic violation
    NETWORK_ERROR,         // Network/connectivity issues
    RESOURCE_LOCKED,       // Resource currently locked/busy
    INSUFFICIENT_PERMISSIONS, // User lacks required permissions
    SYSTEM_ERROR          // Unexpected system error
};

/**
 * @brief Configuration for use case operations
 */
struct OperationConfig {
    std::chrono::milliseconds timeout{30000};  // 30 second default timeout
    int maxRetries{3};                         // Maximum retry attempts
    std::chrono::milliseconds retryDelay{1000}; // Delay between retries
    bool enableProgressCallback{false};        // Enable progress reporting
    
    OperationConfig() = default;
    
    OperationConfig(std::chrono::milliseconds timeoutMs, int retries = 3)
        : timeout(timeoutMs), maxRetries(retries) {}
};

/**
 * @brief Progress information for long-running operations
 */
struct OperationProgress {
    std::string operation;        // Current operation description
    int currentStep{0};          // Current step number
    int totalSteps{1};           // Total number of steps
    double percentComplete{0.0}; // Percentage complete (0.0-100.0)
    std::string statusMessage;   // Current status message
    
    OperationProgress(const std::string& op = "") : operation(op) {}
};

/**
 * @brief Callback function type for progress reporting
 */
using ProgressCallback = std::function<void(const OperationProgress&)>;

/**
 * @brief Enhanced error information
 */
struct ErrorDetails {
    ErrorCategory category;
    std::string message;
    std::string technicalDetails; // For logging/debugging
    int retryAttempt{0};          // Which retry attempt failed
    
    ErrorDetails(ErrorCategory cat, const std::string& msg, const std::string& tech = "")
        : category(cat), message(msg), technicalDetails(tech) {}
};

/**
 * @brief Base response structure for all use cases
 */
template<typename T>
struct Response {
    T result;                    // The actual result data
    bool success{false};
    ErrorDetails error{ErrorCategory::VALIDATION_ERROR, ""};
    std::chrono::milliseconds executionTime{0};
    bool timedOut{false};
    
    Response() = default;
    
    // Success constructor
    Response(const T& res) : result(res), success(true) {}
    
    // Error constructor
    Response(ErrorCategory category, const std::string& message, const std::string& technical = "")
        : success(false), error(category, message, technical) {}
};

/**
 * @brief Specialization for void results
 */
template<>
struct Response<void> {
    bool success{false};
    ErrorDetails error{ErrorCategory::VALIDATION_ERROR, ""};
    std::chrono::milliseconds executionTime{0};
    bool timedOut{false};
    
    Response() = default;
    
    // Success constructor
    Response(bool) : success(true) {}
    
    // Error constructor
    Response(ErrorCategory category, const std::string& message, const std::string& technical = "")
        : success(false), error(category, message, technical) {}
};

/**
 * @brief Helper to convert error category to string
 */
inline std::string errorCategoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::VALIDATION_ERROR: return "Validation Error";
        case ErrorCategory::TIMEOUT_ERROR: return "Timeout Error";
        case ErrorCategory::REPOSITORY_ERROR: return "Repository Error";
        case ErrorCategory::BUSINESS_RULE_ERROR: return "Business Rule Error";
        case ErrorCategory::NETWORK_ERROR: return "Network Error";
        case ErrorCategory::RESOURCE_LOCKED: return "Resource Locked";
        case ErrorCategory::INSUFFICIENT_PERMISSIONS: return "Insufficient Permissions";
        case ErrorCategory::SYSTEM_ERROR: return "System Error";
        default: return "Unknown Error";
    }
}

} // namespace UseCase

#endif // USECASECOMMON_H