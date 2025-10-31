#ifndef BASEUSECASE_H
#define BASEUSECASE_H

#include "UseCaseCommon.h"
#include <future>
#include <thread>
#include <chrono>
#include <functional>

namespace UseCase {

/**
 * @brief Base class for all use cases providing common functionality
 * 
 * This class provides:
 * - Timeout handling for long-running operations
 * - Retry logic for transient failures
 * - Progress reporting for complex operations
 * - Consistent error handling
 */
class BaseUseCase {
protected:
    OperationConfig defaultConfig;
    
public:
    explicit BaseUseCase(const OperationConfig& config = OperationConfig())
        : defaultConfig(config) {}
    
    virtual ~BaseUseCase() = default;

protected:
    /**
     * @brief Execute an operation with timeout and retry logic
     * 
     * @tparam T The return type of the operation
     * @param operation The operation to execute
     * @param config Operation configuration
     * @param progressCallback Optional progress callback
     * @return Response with result or error details
     */
    template<typename T>
    Response<T> executeWithRetry(
        std::function<T()> operation,
        const OperationConfig& config = OperationConfig(),
        ProgressCallback progressCallback = nullptr
    ) {
        auto startTime = std::chrono::steady_clock::now();
        auto actualConfig = (config.timeout.count() > 0) ? config : defaultConfig;
        
        for (int attempt = 0; attempt <= actualConfig.maxRetries; ++attempt) {
            try {
                // Report progress if callback provided
                if (progressCallback) {
                    OperationProgress progress("Executing operation");
                    progress.currentStep = attempt + 1;
                    progress.totalSteps = actualConfig.maxRetries + 1;
                    progress.statusMessage = (attempt == 0) ? "First attempt" : 
                                           "Retry attempt " + std::to_string(attempt);
                    progressCallback(progress);
                }
                
                // Execute with timeout
                auto result = executeWithTimeout(operation, actualConfig.timeout);
                
                // Calculate execution time
                auto endTime = std::chrono::steady_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endTime - startTime);
                
                Response<T> response(result);
                response.executionTime = executionTime;
                return response;
                
            } catch (const TimeoutException& e) {
                auto endTime = std::chrono::steady_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endTime - startTime);
                
                Response<T> response(ErrorCategory::TIMEOUT_ERROR, 
                                   "Operation timed out after " + 
                                   std::to_string(actualConfig.timeout.count()) + "ms",
                                   e.what());
                response.executionTime = executionTime;
                response.timedOut = true;
                return response;
                
            } catch (const RepositoryException& e) {
                // Repository errors might be transient, retry them
                if (attempt < actualConfig.maxRetries) {
                    if (progressCallback) {
                        OperationProgress progress("Retrying after repository error");
                        progress.statusMessage = "Waiting " + 
                                               std::to_string(actualConfig.retryDelay.count()) + 
                                               "ms before retry " + std::to_string(attempt + 1);
                        progressCallback(progress);
                    }
                    
                    std::this_thread::sleep_for(actualConfig.retryDelay);
                    continue;
                }
                
                // Max retries exceeded
                auto endTime = std::chrono::steady_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endTime - startTime);
                
                Response<T> response(ErrorCategory::REPOSITORY_ERROR,
                                   "Repository operation failed after " + 
                                   std::to_string(actualConfig.maxRetries + 1) + " attempts",
                                   e.what());
                response.executionTime = executionTime;
                response.error.retryAttempt = attempt;
                return response;
                
            } catch (const ValidationException& e) {
                // Validation errors shouldn't be retried
                auto endTime = std::chrono::steady_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endTime - startTime);
                
                Response<T> response(ErrorCategory::VALIDATION_ERROR, e.what());
                response.executionTime = executionTime;
                return response;
                
            } catch (const BusinessRuleException& e) {
                // Business rule violations shouldn't be retried
                auto endTime = std::chrono::steady_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endTime - startTime);
                
                Response<T> response(ErrorCategory::BUSINESS_RULE_ERROR, e.what());
                response.executionTime = executionTime;
                return response;
                
            } catch (const std::exception& e) {
                // Unknown errors - retry with backoff
                if (attempt < actualConfig.maxRetries) {
                    std::this_thread::sleep_for(actualConfig.retryDelay);
                    continue;
                }
                
                auto endTime = std::chrono::steady_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endTime - startTime);
                
                Response<T> response(ErrorCategory::SYSTEM_ERROR,
                                   "Unexpected error after " + 
                                   std::to_string(actualConfig.maxRetries + 1) + " attempts",
                                   e.what());
                response.executionTime = executionTime;
                response.error.retryAttempt = attempt;
                return response;
            }
        }
        
        // This shouldn't be reached, but just in case
        Response<T> response(ErrorCategory::SYSTEM_ERROR, "Maximum retries exceeded");
        return response;
    }

private:
    /**
     * @brief Custom exception types for different error categories
     */
    class TimeoutException : public std::runtime_error {
    public:
        explicit TimeoutException(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    class RepositoryException : public std::runtime_error {
    public:
        explicit RepositoryException(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    class ValidationException : public std::runtime_error {
    public:
        explicit ValidationException(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    class BusinessRuleException : public std::runtime_error {
    public:
        explicit BusinessRuleException(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    /**
     * @brief Execute operation with timeout
     */
    template<typename T>
    T executeWithTimeout(std::function<T()> operation, std::chrono::milliseconds timeout) {
        auto future = std::async(std::launch::async, operation);
        
        if (future.wait_for(timeout) == std::future_status::timeout) {
            throw TimeoutException("Operation exceeded timeout of " + 
                                 std::to_string(timeout.count()) + "ms");
        }
        
        return future.get();
    }

protected:
    /**
     * @brief Helper to throw appropriate exception types
     */
    void throwValidationError(const std::string& message) {
        throw ValidationException(message);
    }
    
    void throwBusinessRuleError(const std::string& message) {
        throw BusinessRuleException(message);
    }
    
    void throwRepositoryError(const std::string& message) {
        throw RepositoryException(message);
    }
};

} // namespace UseCase

#endif // BASEUSECASE_H