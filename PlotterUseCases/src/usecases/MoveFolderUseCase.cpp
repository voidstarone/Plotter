#include "usecases/MoveFolderUseCase.h"
#include <stdexcept>

MoveFolderUseCase::MoveFolderUseCase(
    std::shared_ptr<FolderRepository> folderRepo,
    std::shared_ptr<ProjectRepository> projectRepo
) : folderRepository(folderRepo), projectRepository(projectRepo) {
    if (!folderRepository) {
        throw std::invalid_argument("FolderRepository cannot be null");
    }
    if (!projectRepository) {
        throw std::invalid_argument("ProjectRepository cannot be null");
    }
}

MoveFolderUseCase::Response MoveFolderUseCase::execute(const Request& request) {
    Response response;
    
    // Validate request
    std::string validationError = validateRequest(request);
    if (!validationError.empty()) {
        response.success = false;
        response.errorMessage = validationError;
        return response;
    }
    
    try {
        // Get the folder to move
        auto folder = folderRepository->findById(request.folderId);
        if (!folder.has_value()) {
            response.success = false;
            response.errorMessage = "Folder not found: " + request.folderId;
            return response;
        }
        
        // Remove folder from old parent
        std::string oldParentProjectId = folder->getParentProjectId();
        std::string oldParentFolderId = folder->getParentFolderId();
        
        if (!oldParentProjectId.empty()) {
            auto oldProject = projectRepository->findById(oldParentProjectId);
            if (oldProject.has_value()) {
                Project updatedProject = oldProject.value();
                updatedProject.removeFolderId(request.folderId);
                projectRepository->update(updatedProject);
            }
        }
        
        if (!oldParentFolderId.empty()) {
            auto oldParentFolder = folderRepository->findById(oldParentFolderId);
            if (oldParentFolder.has_value()) {
                Folder updatedOldParent = oldParentFolder.value();
                updatedOldParent.removeSubfolderId(request.folderId);
                folderRepository->update(updatedOldParent);
            }
        }
        
        // Update folder with new parent
        Folder updatedFolder = folder.value();
        updatedFolder.setParentProjectId(request.newParentProjectId);
        updatedFolder.setParentFolderId(request.newParentFolderId);
        
        // Add folder to new parent
        if (!request.newParentProjectId.empty()) {
            auto newProject = projectRepository->findById(request.newParentProjectId);
            if (newProject.has_value()) {
                Project updatedProject = newProject.value();
                updatedProject.addFolderId(request.folderId);
                projectRepository->update(updatedProject);
            }
        }
        
        if (!request.newParentFolderId.empty()) {
            auto newParentFolder = folderRepository->findById(request.newParentFolderId);
            if (newParentFolder.has_value()) {
                Folder updatedNewParent = newParentFolder.value();
                updatedNewParent.addSubfolderId(request.folderId);
                folderRepository->update(updatedNewParent);
            }
        }
        
        // Save updated folder
        folderRepository->update(updatedFolder);
        
        response.success = true;
        return response;
        
    } catch (const std::exception& e) {
        response.success = false;
        response.errorMessage = "Failed to move folder: " + std::string(e.what());
        return response;
    }
}

std::string MoveFolderUseCase::validateRequest(const Request& request) {
    if (request.folderId.empty()) {
        return "Folder ID cannot be empty";
    }
    
    // Either newParentProjectId or newParentFolderId must be specified
    if (request.newParentProjectId.empty() && request.newParentFolderId.empty()) {
        return "Either newParentProjectId or newParentFolderId must be specified";
    }
    
    // Both cannot be specified
    if (!request.newParentProjectId.empty() && !request.newParentFolderId.empty()) {
        return "Cannot specify both newParentProjectId and newParentFolderId";
    }
    
    // Validate that new parent project exists (if specified)
    if (!request.newParentProjectId.empty()) {
        if (!projectRepository->exists(request.newParentProjectId)) {
            return "New parent project does not exist: " + request.newParentProjectId;
        }
    }
    
    // Validate that new parent folder exists (if specified)
    if (!request.newParentFolderId.empty()) {
        if (!folderRepository->exists(request.newParentFolderId)) {
            return "New parent folder does not exist: " + request.newParentFolderId;
        }
    }
    
    return ""; // Valid
}