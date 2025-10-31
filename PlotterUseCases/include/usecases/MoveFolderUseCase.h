#ifndef MOVEFOLDERUSECASE_H
#define MOVEFOLDERUSECASE_H

#include "repositories/FolderRepository.h"
#include "repositories/ProjectRepository.h"
#include <memory>
#include <string>

/**
 * @brief Use case for moving a folder to a new parent
 * 
 * This use case handles the business logic for moving a folder from one
 * parent to another, updating all the necessary relationships.
 */
class MoveFolderUseCase {
public:
    /**
     * @brief Request DTO for moving a folder
     */
    struct Request {
        std::string folderId;
        std::string newParentProjectId;  // Optional, for moving to project root
        std::string newParentFolderId;   // Optional, for moving to another folder
    };

    /**
     * @brief Response DTO for folder move
     */
    struct Response {
        bool success;
        std::string errorMessage;
    };

private:
    std::shared_ptr<FolderRepository> folderRepository;
    std::shared_ptr<ProjectRepository> projectRepository;

public:
    /**
     * @brief Constructor
     * 
     * @param folderRepo The folder repository
     * @param projectRepo The project repository
     */
    MoveFolderUseCase(
        std::shared_ptr<FolderRepository> folderRepo,
        std::shared_ptr<ProjectRepository> projectRepo
    );
    
    /**
     * @brief Execute the use case
     * 
     * @param request The request containing move details
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

#endif // MOVEFOLDERUSECASE_H