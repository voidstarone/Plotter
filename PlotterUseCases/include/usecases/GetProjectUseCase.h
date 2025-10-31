#ifndef GETPROJECTUSECASE_H
#define GETPROJECTUSECASE_H

#include "repositories/ProjectRepository.h"
#include "Project.h"
#include <memory>
#include <string>
#include <optional>

/**
 * @brief Use case for retrieving a project by ID
 */
class GetProjectUseCase {
public:
    /**
     * @brief Request DTO for getting a project
     */
    struct Request {
        std::string projectId;
    };

    /**
     * @brief Response DTO for project retrieval
     */
    struct Response {
        std::optional<Project> project;
        bool success;
        std::string errorMessage;
    };

private:
    std::shared_ptr<ProjectRepository> projectRepository;

public:
    /**
     * @brief Constructor
     * 
     * @param repo The project repository to use for retrieval
     */
    explicit GetProjectUseCase(std::shared_ptr<ProjectRepository> repo);
    
    /**
     * @brief Execute the use case
     * 
     * @param request The request containing project ID
     * @return Response containing the result of the operation
     */
    Response execute(const Request& request);

private:
    /**
     * @brief Validate the request
     * 
     * @param request The request to validate
     * @return Error message if validation fails, empty string if valid
     */
    std::string validateRequest(const Request& request);
};

#endif // GETPROJECTUSECASE_H