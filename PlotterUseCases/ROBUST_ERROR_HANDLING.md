# Robust Use Cases - Production-Ready Error Handling

## Overview

The PlotterUseCases library has been enhanced with production-ready error handling that accounts for real-world conditions where operations can take time and fail in various ways.

## Key Enhancements

### 1. **Timeout Protection** ⏱️
Operations that might hang due to network issues, database locks, or slow storage are protected with configurable timeouts.

```cpp
CreateProjectUseCase::Request request;
request.name = "My Project";
request.config.timeout = std::chrono::milliseconds(5000); // 5 second timeout

auto response = createProject->execute(request);
if (response.timedOut) {
    std::cout << "Operation timed out after 5 seconds" << std::endl;
}
```

### 2. **Retry Logic** 🔄
Transient failures (network glitches, temporary database unavailability) are automatically retried with configurable backoff.

```cpp
request.config.maxRetries = 3; // Try up to 3 times
request.config.retryDelay = std::chrono::milliseconds(1000); // 1 second between retries

auto response = createProject->execute(request);
if (!response.success) {
    std::cout << "Failed after " << response.error.retryAttempt << " retries" << std::endl;
}
```

### 3. **Error Categories** 🏷️
Detailed error categorization helps applications respond appropriately to different types of failures.

```cpp
if (!response.success) {
    switch (response.error.category) {
        case UseCase::ErrorCategory::VALIDATION_ERROR:
            // Fix input and don't retry
            break;
        case UseCase::ErrorCategory::TIMEOUT_ERROR:
            // Maybe try again with longer timeout
            break;
        case UseCase::ErrorCategory::REPOSITORY_ERROR:
            // Check network/database connection
            break;
        case UseCase::ErrorCategory::RESOURCE_LOCKED:
            // Wait and try again later
            break;
    }
}
```

### 4. **Progress Reporting** 📊
Long-running operations provide progress feedback for better user experience.

```cpp
request.progressCallback = [](const UseCase::OperationProgress& progress) {
    std::cout << progress.operation 
              << " [" << progress.currentStep << "/" << progress.totalSteps << "] "
              << "(" << (int)progress.percentComplete << "%) - "
              << progress.statusMessage << std::endl;
};

// Output:
// Creating project [1/3] (0%) - Generating unique project ID
// Creating project [2/3] (33%) - Creating project entity  
// Creating project [3/3] (66%) - Persisting to repository
// Creating project [3/3] (100%) - Project created successfully
```

### 5. **Transaction-Like Behavior** 🔒
Complex operations that can fail partially support rollback mechanisms.

```cpp
DeleteProjectUseCase::Request request;
request.projectId = "project-123";
request.performRollbackOnFailure = true; // Rollback on partial failure

auto response = deleteProject->execute(request);
if (response.success && response.result.wasPartialDeletion) {
    std::cout << "Operation completed but required rollback" << std::endl;
    std::cout << "Rolled back: " << response.result.rolledBackItems.size() << " items" << std::endl;
}
```

## Error Categories

| Category | Description | Retry Behavior | Use Case |
|----------|-------------|----------------|----------|
| `VALIDATION_ERROR` | Input validation failed | No retry | Fix input data |
| `TIMEOUT_ERROR` | Operation exceeded time limit | No retry | Increase timeout or check performance |
| `REPOSITORY_ERROR` | Database/storage failure | Retry with backoff | Transient network/DB issues |
| `BUSINESS_RULE_ERROR` | Business logic violation | No retry | Fix business logic |
| `NETWORK_ERROR` | Network connectivity issue | Retry with backoff | Temporary network problems |
| `RESOURCE_LOCKED` | Resource currently locked/busy | Retry with backoff | Concurrent access conflicts |
| `INSUFFICIENT_PERMISSIONS` | User lacks required permissions | No retry | Fix permissions |
| `SYSTEM_ERROR` | Unexpected system error | Retry with backoff | Unknown transient issues |

## Configuration Options

### OperationConfig Structure
```cpp
struct OperationConfig {
    std::chrono::milliseconds timeout{30000};     // 30 second default timeout
    int maxRetries{3};                            // Maximum retry attempts
    std::chrono::milliseconds retryDelay{1000};   // Delay between retries
    bool enableProgressCallback{false};           // Enable progress reporting
};
```

### Common Configurations

**Fast Operations** (simple CRUD):
```cpp
UseCase::OperationConfig fastConfig;
fastConfig.timeout = std::chrono::milliseconds(5000);  // 5 seconds
fastConfig.maxRetries = 2;
fastConfig.retryDelay = std::chrono::milliseconds(500); // 0.5 seconds
```

**Long Operations** (complex processing):
```cpp
UseCase::OperationConfig longConfig;
longConfig.timeout = std::chrono::milliseconds(300000); // 5 minutes  
longConfig.maxRetries = 5;
longConfig.retryDelay = std::chrono::milliseconds(2000); // 2 seconds
longConfig.enableProgressCallback = true;
```

**Critical Operations** (no retries):
```cpp
UseCase::OperationConfig criticalConfig;
criticalConfig.timeout = std::chrono::milliseconds(10000); // 10 seconds
criticalConfig.maxRetries = 0; // No retries for critical operations
```

## Response Structure

All use cases now return enhanced responses with detailed information:

```cpp
template<typename T>
struct Response {
    T result;                                    // The actual result data
    bool success{false};                         // Operation success flag
    ErrorDetails error;                          // Detailed error information
    std::chrono::milliseconds executionTime{0}; // How long it took
    bool timedOut{false};                        // Whether it timed out
};

struct ErrorDetails {
    ErrorCategory category;      // Error category
    std::string message;         // User-friendly message
    std::string technicalDetails; // Technical details for logging
    int retryAttempt{0};         // Which retry attempt failed
};
```

## Usage Examples

### Basic Robust Operation
```cpp
auto createProject = std::make_shared<CreateProjectUseCase>(projectRepo);

CreateProjectUseCase::Request request;
request.name = "My Project";
request.description = "A robust project";

auto response = createProject->execute(request);

if (response.success) {
    std::cout << "Created project: " << response.result.projectId << std::endl;
    std::cout << "Took: " << response.executionTime.count() << "ms" << std::endl;
} else {
    std::cout << "Failed: " << response.error.message << std::endl;
    std::cout << "Category: " << errorCategoryToString(response.error.category) << std::endl;
    
    // Log technical details for debugging
    logger.error("Project creation failed", {
        {"category", response.error.category},
        {"technical", response.error.technicalDetails},
        {"retryAttempt", response.error.retryAttempt},
        {"executionTime", response.executionTime.count()}
    });
}
```

### Operation with Custom Configuration
```cpp
CreateProjectUseCase::Request request;
request.name = "High Priority Project";
request.description = "This project is critical";

// Custom configuration for important operation
request.config.timeout = std::chrono::milliseconds(60000); // 1 minute
request.config.maxRetries = 5; // Try harder
request.config.retryDelay = std::chrono::milliseconds(2000); // Wait longer between retries

// Progress reporting for user feedback
request.progressCallback = [&](const UseCase::OperationProgress& progress) {
    updateUI(progress.percentComplete, progress.statusMessage);
};

auto response = createProject->execute(request);
```

### Complex Operation with Rollback
```cpp
DeleteProjectUseCase::Request request;
request.projectId = "large-project-123";
request.performRollbackOnFailure = true;

// Long timeout for large project
request.config.timeout = std::chrono::milliseconds(600000); // 10 minutes
request.config.maxRetries = 3;

// Progress reporting for long operation
request.progressCallback = [&](const UseCase::OperationProgress& progress) {
    std::cout << "Deleting: " << progress.statusMessage 
              << " (" << (int)progress.percentComplete << "%)" << std::endl;
};

auto response = deleteProject->execute(request);

if (response.success) {
    std::cout << "Deleted " << response.result.deletedFolders << " folders" << std::endl;
    std::cout << "Deleted " << response.result.deletedNotes << " notes" << std::endl;
    
    if (response.result.wasPartialDeletion) {
        std::cout << "Warning: Partial deletion occurred but was handled" << std::endl;
    }
} else {
    std::cout << "Deletion failed: " << response.error.message << std::endl;
    
    if (response.result.failedDeletions.size() > 0) {
        std::cout << "Failed to delete " << response.result.failedDeletions.size() << " items" << std::endl;
    }
}
```

## Architecture Benefits

### 1. **Production Ready**
- Operations don't hang indefinitely
- Transient failures are handled gracefully  
- Users get meaningful feedback on long operations
- Partial failures are handled correctly

### 2. **Observability**
- Detailed execution metrics
- Categorized error information
- Progress tracking for complex operations
- Technical details for debugging

### 3. **Reliability**
- Automatic retry for transient issues
- Timeout protection against hanging
- Rollback capabilities for complex operations
- Consistent error handling across all use cases

### 4. **User Experience**
- Progress feedback for long operations
- Clear error messages for different failure types
- Configurable timeouts based on operation criticality
- Graceful degradation on partial failures

This robust error handling transforms the use cases from simple demo code into production-ready application services that can handle the complexities of real-world systems.