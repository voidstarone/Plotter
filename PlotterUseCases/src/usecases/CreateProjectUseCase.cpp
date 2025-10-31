#include "usecases/CreateProjectUseCase.h"
#include "Project.h"
#include <chrono>
#include <stdexcept>

CreateProjectUseCase::CreateProjectUseCase(
    std::shared_ptr<ProjectRepository> repo,
    const UseCase::OperationConfig& defaultConfig
) : BaseUseCase(defaultConfig), projectRepository(repo) {
    if (!projectRepository) {
        throw std::invalid_argument("ProjectRepository cannot be null");
    }
}

CreateProjectUseCase::Response CreateProjectUseCase::execute(const Request& request) {
    // Validate request first (no retry needed for validation)
    std::string validationError = validateRequest(request);
    if (!validationError.empty()) {
        return Response(UseCase::ErrorCategory::VALIDATION_ERROR, validationError);
    }
    
    // Execute the actual operation with timeout and retry logic
    auto operation = [this, &request]() -> CreateProjectResult {
        // Report progress if callback provided
        if (request.progressCallback) {
            UseCase::OperationProgress progress("Creating project");
            progress.currentStep = 1;
            progress.totalSteps = 3;
            progress.percentComplete = 0.0;
            progress.statusMessage = "Generating unique project ID";
            request.progressCallback(progress);
        }
        
        // Generate unique ID
        std::string id = generateProjectId();
        
        if (request.progressCallback) {
            UseCase::OperationProgress progress("Creating project");
            progress.currentStep = 2;
            progress.totalSteps = 3;
            progress.percentComplete = 33.0;
            progress.statusMessage = "Creating project entity";
            request.progressCallback(progress);
        }
        
        // Create entity
        Project project(id, request.name, request.description);
        
        if (request.progressCallback) {
            UseCase::OperationProgress progress("Creating project");
            progress.currentStep = 3;
            progress.totalSteps = 3;
            progress.percentComplete = 66.0;
            progress.statusMessage = "Persisting to repository";
            request.progressCallback(progress);
        }
        
        // Persist via repository (this might fail or take time)
        try {
            std::string savedId = projectRepository->save(project);
            
            if (request.progressCallback) {
                UseCase::OperationProgress progress("Creating project");
                progress.currentStep = 3;
                progress.totalSteps = 3;
                progress.percentComplete = 100.0;
                progress.statusMessage = "Project created successfully";
                request.progressCallback(progress);
            }
            
            return CreateProjectResult(savedId, request.name);
            
        } catch (const std::exception& e) {
            // Convert repository exceptions to our exception types
            throwRepositoryError("Failed to save project to repository: " + std::string(e.what()));
        }
        
        // This line should never be reached due to exception above
        throw std::runtime_error("Unexpected code path");
    };
    
    // Execute with retry and timeout handling
    return executeWithRetry<CreateProjectResult>(operation, request.config, request.progressCallback);
}

std::string CreateProjectUseCase::generateProjectId() {
    // In a real system, you'd use a UUID library or proper ID generation service
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return "proj_" + std::to_string(millis);
}

std::string CreateProjectUseCase::validateRequest(const Request& request) {
    if (request.name.empty()) {
        return "Project name cannot be empty";
    }
    
    if (request.name.length() > 255) {
        return "Project name cannot exceed 255 characters (current: " + 
               std::to_string(request.name.length()) + ")";
    }
    
    if (request.description.length() > 1000) {
        return "Project description cannot exceed 1000 characters (current: " + 
               std::to_string(request.description.length()) + ")";
    }
    
    // Validate configuration
    if (request.config.timeout.count() < 0) {
        return "Timeout cannot be negative";
    }
    
    if (request.config.maxRetries < 0) {
        return "Max retries cannot be negative";
    }
    
    if (request.config.retryDelay.count() < 0) {
        return "Retry delay cannot be negative";
    }
    
    return ""; // Valid
}