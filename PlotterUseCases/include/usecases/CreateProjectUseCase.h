#ifndef CREATEPROJECTUSECASE_H
#define CREATEPROJECTUSECASE_H

#include "repositories/ProjectRepository.h"
#include "BaseUseCase.h"
#include "UseCaseCommon.h"
#include <memory>
#include <string>

/**
 * @brief Use case for creating a new project
 * 
 * This use case handles the business logic for creating a new project,
 * including validation and persistence through the ProjectRepository.
 * 
 * Features:
 * - Timeout handling for slow repository operations
 * - Retry logic for transient failures
 * - Detailed error categorization
 * - Execution time tracking
 */
class CreateProjectUseCase : public UseCase::BaseUseCase {
public:
    /**
     * @brief Request DTO for creating a project
     */
    struct Request {
        std::string name;
        std::string description;
        UseCase::OperationConfig config; // Timeout and retry configuration
        UseCase::ProgressCallback progressCallback; // Optional progress reporting
    };

    /**
     * @brief Result data for successful project creation
     */
    struct CreateProjectResult {
        std::string projectId;
        std::string projectName;
        std::chrono::system_clock::time_point createdAt;
        
        CreateProjectResult() = default;
        CreateProjectResult(const std::string& id, const std::string& name)
            : projectId(id), projectName(name), createdAt(std::chrono::system_clock::now()) {}
    };

    /**
     * @brief Response DTO for project creation using enhanced error handling
     */
    using Response = UseCase::Response<CreateProjectResult>;

private:
    std::shared_ptr<ProjectRepository> projectRepository;

public:
    /**
     * @brief Constructor
     * 
     * @param repo The project repository to use for persistence
     * @param defaultConfig Default operation configuration
     */
    explicit CreateProjectUseCase(
        std::shared_ptr<ProjectRepository> repo,
        const UseCase::OperationConfig& defaultConfig = UseCase::OperationConfig()
    );
    
    /**
     * @brief Execute the use case with robust error handling
     * 
     * This method handles:
     * - Input validation with detailed error messages
     * - Timeout protection for slow repository operations
     * - Automatic retry for transient failures
     * - Progress reporting for long operations
     * - Detailed error categorization
     * 
     * @param request The request containing project details and configuration
     * @return Response containing the result or detailed error information
     */
    Response execute(const Request& request);

private:
    /**
     * @brief Generate a unique ID for the project
     * 
     * @return A unique project ID string
     */
    std::string generateProjectId();
    
    /**
     * @brief Validate the request
     * 
     * @param request The request to validate
     * @return Error message if validation fails, empty string if valid
     */
    std::string validateRequest(const Request& request);
};

#endif // CREATEPROJECTUSECASE_H