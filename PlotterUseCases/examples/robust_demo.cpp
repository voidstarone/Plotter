#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

// Entity includes
#include "Project.h"
#include "Folder.h" 
#include "Note.h"
#include "FileSystemNoteStorage.h"

// Repository interfaces
#include "repositories/ProjectRepository.h"
#include "repositories/FolderRepository.h"
#include "repositories/NoteRepository.h"

// Test mock implementations
#include "InMemoryProjectRepository.h"
#include "InMemoryFolderRepository.h"
#include "InMemoryNoteRepository.h"

// Enhanced use cases
#include "usecases/CreateProjectUseCase.h"
#include "usecases/UseCaseCommon.h"

/**
 * @brief Demo showing robust use cases with timeout, retry, and progress reporting
 */
class RobustUseCasesDemo {
private:
    // Infrastructure layer
    std::shared_ptr<NoteStorage> noteStorage;
    
    // Repository implementations  
    std::shared_ptr<ProjectRepository> projectRepo;
    std::shared_ptr<FolderRepository> folderRepo;
    std::shared_ptr<NoteRepository> noteRepo;
    
    // Enhanced use cases
    std::shared_ptr<CreateProjectUseCase> createProject;

public:
    RobustUseCasesDemo() {
        setupDependencies();
    }
    
    void run() {
        std::cout << "=== Robust Use Cases Demo ===" << std::endl;
        std::cout << "Demonstrating timeout, retry, and progress reporting" << std::endl << std::endl;
        
        demonstrateBasicOperation();
        demonstrateProgressReporting();
        demonstrateTimeoutHandling();
        demonstrateRetryLogic();
        
        std::cout << std::endl << "=== Robust Demo Complete ===" << std::endl;
    }

private:
    void setupDependencies() {
        std::cout << "Setting up robust dependencies..." << std::endl;
        
        // Infrastructure layer
        noteStorage = std::make_shared<FileSystemNoteStorage>("./robust_demo_notes");
        
        // Repository implementations
        projectRepo = std::make_shared<InMemoryProjectRepository>();
        folderRepo = std::make_shared<InMemoryFolderRepository>();
        noteRepo = std::make_shared<InMemoryNoteRepository>(noteStorage);
        
        // Use cases with default configuration
        UseCase::OperationConfig defaultConfig;
        defaultConfig.timeout = std::chrono::milliseconds(10000); // 10 second timeout
        defaultConfig.maxRetries = 2; // 2 retries
        defaultConfig.retryDelay = std::chrono::milliseconds(500); // 0.5s between retries
        
        createProject = std::make_shared<CreateProjectUseCase>(projectRepo, defaultConfig);
        
        std::cout << "Robust dependencies configured!" << std::endl << std::endl;
    }
    
    void demonstrateBasicOperation() {
        std::cout << "1. Basic operation with enhanced response..." << std::endl;
        
        CreateProjectUseCase::Request request;
        request.name = "Robust Test Project";
        request.description = "Testing enhanced error handling";
        // Using default config (no custom timeout/retry)
        
        auto response = createProject->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Project created successfully!" << std::endl;
            std::cout << "   ✓ Project ID: " << response.result.projectId << std::endl;
            std::cout << "   ✓ Project Name: " << response.result.projectName << std::endl;
            std::cout << "   ✓ Execution Time: " << response.executionTime.count() << "ms" << std::endl;
        } else {
            std::cout << "   ✗ Failed: " << response.error.message << std::endl;
            std::cout << "   ✗ Category: " << UseCase::errorCategoryToString(response.error.category) << std::endl;
            if (!response.error.technicalDetails.empty()) {
                std::cout << "   ✗ Technical: " << response.error.technicalDetails << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    void demonstrateProgressReporting() {
        std::cout << "2. Operation with progress reporting..." << std::endl;
        
        CreateProjectUseCase::Request request;
        request.name = "Progress Demo Project";
        request.description = "Testing progress callbacks";
        
        // Set up progress callback
        request.progressCallback = [](const UseCase::OperationProgress& progress) {
            std::cout << "   📊 " << progress.operation 
                      << " [" << progress.currentStep << "/" << progress.totalSteps << "] "
                      << "(" << (int)progress.percentComplete << "%) - "
                      << progress.statusMessage << std::endl;
        };
        
        auto response = createProject->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Project created with progress tracking!" << std::endl;
            std::cout << "   ✓ Total execution time: " << response.executionTime.count() << "ms" << std::endl;
        } else {
            std::cout << "   ✗ Failed: " << response.error.message << std::endl;
        }
        std::cout << std::endl;
    }
    
    void demonstrateTimeoutHandling() {
        std::cout << "3. Operation with timeout configuration..." << std::endl;
        
        CreateProjectUseCase::Request request;
        request.name = "Timeout Test Project";
        request.description = "Testing timeout handling";
        
        // Set a very short timeout to trigger timeout behavior
        request.config.timeout = std::chrono::milliseconds(1); // 1ms - very short!
        request.config.maxRetries = 1;
        
        auto response = createProject->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Project created (operation was fast enough)" << std::endl;
        } else {
            std::cout << "   ⚠ Operation result: " << response.error.message << std::endl;
            std::cout << "   ⚠ Category: " << UseCase::errorCategoryToString(response.error.category) << std::endl;
            if (response.timedOut) {
                std::cout << "   ⚠ Operation timed out as expected" << std::endl;
            }
            std::cout << "   ⚠ Execution time: " << response.executionTime.count() << "ms" << std::endl;
        }
        std::cout << std::endl;
    }
    
    void demonstrateRetryLogic() {
        std::cout << "4. Demonstrating validation (no retry needed)..." << std::endl;
        
        CreateProjectUseCase::Request request;
        request.name = ""; // Invalid name to trigger validation error
        request.description = "Testing validation";
        
        // Configure retries (won't be used for validation errors)
        request.config.maxRetries = 3;
        request.config.retryDelay = std::chrono::milliseconds(100);
        
        auto response = createProject->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Unexpected success" << std::endl;
        } else {
            std::cout << "   ✓ Validation error caught (no retries performed)" << std::endl;
            std::cout << "   ✓ Error: " << response.error.message << std::endl;
            std::cout << "   ✓ Category: " << UseCase::errorCategoryToString(response.error.category) << std::endl;
            std::cout << "   ✓ Execution time: " << response.executionTime.count() << "ms (fast validation)" << std::endl;
            std::cout << "   ✓ Retry attempt: " << response.error.retryAttempt << " (should be 0)" << std::endl;
        }
        std::cout << std::endl;
    }
};

int main() {
    try {
        RobustUseCasesDemo demo;
        demo.run();
    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}