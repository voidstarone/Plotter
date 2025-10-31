#include "usecases/CreateFolderUseCase.h"
#include "Folder.h"
#include <chrono>
#include <stdexcept>

CreateFolderUseCase::CreateFolderUseCase(
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

CreateFolderUseCase::Response CreateFolderUseCase::execute(const Request& request) {
    Response response;
    
    // Validate request
    std::string validationError = validateRequest(request);
    if (!validationError.empty()) {
        response.success = false;
        response.errorMessage = validationError;
        return response;
    }
    
    try {
        // Generate unique ID
        std::string id = generateFolderId();
        
        // Create entity
        Folder folder(id, request.name, request.description, 
                     request.parentProjectId, request.parentFolderId);
        
        // Persist via repository
        std::string savedId = folderRepository->save(folder);
        
        // Update parent project or folder to include this new folder
        if (!request.parentProjectId.empty()) {
            auto project = projectRepository->findById(request.parentProjectId);
            if (project.has_value()) {
                Project updatedProject = project.value();
                updatedProject.addFolderId(savedId);
                projectRepository->update(updatedProject);
            }
        }
        
        if (!request.parentFolderId.empty()) {
            auto parentFolder = folderRepository->findById(request.parentFolderId);
            if (parentFolder.has_value()) {
                Folder updatedParentFolder = parentFolder.value();
                updatedParentFolder.addSubfolderId(savedId);
                folderRepository->update(updatedParentFolder);
            }
        }
        
        response.success = true;
        response.folderId = savedId;
        return response;
        
    } catch (const std::exception& e) {
        response.success = false;
        response.errorMessage = "Failed to create folder: " + std::string(e.what());
        return response;
    }
}

std::string CreateFolderUseCase::generateFolderId() {
    // In a real system, you'd use a UUID library or proper ID generation service
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return "folder_" + std::to_string(millis);
}

std::string CreateFolderUseCase::validateRequest(const Request& request) {
    if (request.name.empty()) {
        return "Folder name cannot be empty";
    }
    
    if (request.name.length() > 255) {
        return "Folder name cannot exceed 255 characters";
    }
    
    if (request.description.length() > 1000) {
        return "Folder description cannot exceed 1000 characters";
    }
    
    // Either parentProjectId or parentFolderId must be specified
    if (request.parentProjectId.empty() && request.parentFolderId.empty()) {
        return "Either parentProjectId or parentFolderId must be specified";
    }
    
    // Validate that parent project exists (if specified)
    if (!request.parentProjectId.empty()) {
        if (!projectRepository->exists(request.parentProjectId)) {
            return "Parent project does not exist: " + request.parentProjectId;
        }
    }
    
    // Validate that parent folder exists (if specified)
    if (!request.parentFolderId.empty()) {
        if (!folderRepository->exists(request.parentFolderId)) {
            return "Parent folder does not exist: " + request.parentFolderId;
        }
    }
    
    return ""; // Valid
}