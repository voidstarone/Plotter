#include "usecases/DeleteProjectUseCase.h"
#include <stdexcept>

DeleteProjectUseCase::DeleteProjectUseCase(
    std::shared_ptr<ProjectRepository> projectRepo,
    std::shared_ptr<FolderRepository> folderRepo,
    std::shared_ptr<NoteRepository> noteRepo
) : projectRepository(projectRepo), folderRepository(folderRepo), noteRepository(noteRepo) {
    if (!projectRepository) {
        throw std::invalid_argument("ProjectRepository cannot be null");
    }
    if (!folderRepository) {
        throw std::invalid_argument("FolderRepository cannot be null");
    }
    if (!noteRepository) {
        throw std::invalid_argument("NoteRepository cannot be null");
    }
}

DeleteProjectUseCase::Response DeleteProjectUseCase::execute(const Request& request) {
    Response response;
    response.deletedFolders = 0;
    response.deletedNotes = 0;
    
    // Validate request
    std::string validationError = validateRequest(request);
    if (!validationError.empty()) {
        response.success = false;
        response.errorMessage = validationError;
        return response;
    }
    
    try {
        // Check if project exists
        auto project = projectRepository->findById(request.projectId);
        if (!project.has_value()) {
            response.success = false;
            response.errorMessage = "Project not found: " + request.projectId;
            return response;
        }
        
        // Delete all folders in the project (and their contents recursively)
        for (const std::string& folderId : project->getFolderIds()) {
            deleteFolderRecursively(folderId, response.deletedFolders, response.deletedNotes);
        }
        
        // Finally, delete the project itself
        bool projectDeleted = projectRepository->deleteById(request.projectId);
        if (!projectDeleted) {
            response.success = false;
            response.errorMessage = "Failed to delete project from repository";
            return response;
        }
        
        response.success = true;
        return response;
        
    } catch (const std::exception& e) {
        response.success = false;
        response.errorMessage = "Failed to delete project: " + std::string(e.what());
        return response;
    }
}

std::string DeleteProjectUseCase::validateRequest(const Request& request) {
    if (request.projectId.empty()) {
        return "Project ID cannot be empty";
    }
    
    return ""; // Valid
}

void DeleteProjectUseCase::deleteFolderRecursively(const std::string& folderId, int& deletedFolders, int& deletedNotes) {
    // Get the folder
    auto folder = folderRepository->findById(folderId);
    if (!folder.has_value()) {
        return; // Folder doesn't exist, nothing to delete
    }
    
    // Delete all notes in this folder
    for (const std::string& noteId : folder->getNoteIds()) {
        if (noteRepository->deleteById(noteId)) {
            deletedNotes++;
        }
    }
    
    // Recursively delete all subfolders
    for (const std::string& subfolderId : folder->getSubfolderIds()) {
        deleteFolderRecursively(subfolderId, deletedFolders, deletedNotes);
    }
    
    // Finally, delete this folder
    if (folderRepository->deleteById(folderId)) {
        deletedFolders++;
    }
}