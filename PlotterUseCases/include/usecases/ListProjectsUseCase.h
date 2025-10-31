#ifndef LISTPROJECTSUSECASE_H
#define LISTPROJECTSUSECASE_H

#include "repositories/ProjectRepository.h"
#include "Project.h"
#include <memory>
#include <vector>

/**
 * @brief Use case for listing all projects
 */
class ListProjectsUseCase {
public:
    /**
     * @brief Request DTO for listing projects (no parameters needed)
     */
    struct Request {
        // No parameters needed for listing all projects
    };

    /**
     * @brief Response DTO for project listing
     */
    struct Response {
        std::vector<Project> projects;
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
    explicit ListProjectsUseCase(std::shared_ptr<ProjectRepository> repo);
    
    /**
     * @brief Execute the use case
     * 
     * @param request The request (empty for list all)
     * @return Response containing the result of the operation
     */
    Response execute(const Request& request);
};

#endif // LISTPROJECTSUSECASE_H