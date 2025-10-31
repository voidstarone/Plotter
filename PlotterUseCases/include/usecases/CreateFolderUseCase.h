#ifndef CREATEFOLDERUSECASE_H
#define CREATEFOLDERUSECASE_H

#include "repositories/FolderRepository.h"
#include "repositories/ProjectRepository.h"
#include <memory>
#include <string>

/**
 * @brief Use case for creating a new folder
 * 
 * This use case handles the business logic for creating a new folder,
 * including validation and linking to parent project/folder.
 */
class CreateFolderUseCase {
public:
    /**
     * @brief Request DTO for creating a folder
     */
    struct Request {
        std::string name;
        std::string description;
        std::string parentProjectId;  // Required if top-level folder
        std::string parentFolderId;   // Optional, for subfolders
    };

    /**
     * @brief Response DTO for folder creation
     */
    struct Response {
        std::string folderId;
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
     * @param folderRepo The folder repository to use for persistence
     * @param projectRepo The project repository to validate parent project
     */
    CreateFolderUseCase(
        std::shared_ptr<FolderRepository> folderRepo,
        std::shared_ptr<ProjectRepository> projectRepo
    );
    
    /**
     * @brief Execute the use case
     * 
     * @param request The request containing folder details
     * @return Response containing the result of the operation
     */
    Response execute(const Request& request);

private:
    /**
     * @brief Generate a unique ID for the folder
     * 
     * @return A unique folder ID string
     */
    std::string generateFolderId();
    
    /**
     * @brief Validate the request
     * 
     * @param request The request to validate
     * @return Error message if validation fails, empty string if valid
     */
    std::string validateRequest(const Request& request);
};

#endif // CREATEFOLDERUSECASE_H