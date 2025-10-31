#include "usecases/MoveNoteUseCase.h"
#include <stdexcept>

MoveNoteUseCase::MoveNoteUseCase(
    std::shared_ptr<NoteRepository> noteRepo,
    std::shared_ptr<FolderRepository> folderRepo
) : noteRepository(noteRepo), folderRepository(folderRepo) {
    if (!noteRepository) {
        throw std::invalid_argument("NoteRepository cannot be null");
    }
    if (!folderRepository) {
        throw std::invalid_argument("FolderRepository cannot be null");
    }
}

MoveNoteUseCase::Response MoveNoteUseCase::execute(const Request& request) {
    Response response;
    
    // Validate request
    std::string validationError = validateRequest(request);
    if (!validationError.empty()) {
        response.success = false;
        response.errorMessage = validationError;
        return response;
    }
    
    try {
        // Fetch note
        auto note = noteRepository->findById(request.noteId);
        if (!note.has_value()) {
            response.success = false;
            response.errorMessage = "Note not found: " + request.noteId;
            return response;
        }
        
        // Fetch target folder
        auto targetFolder = folderRepository->findById(request.targetFolderId);
        if (!targetFolder.has_value()) {
            response.success = false;
            response.errorMessage = "Target folder not found: " + request.targetFolderId;
            return response;
        }
        
        // Get old folder ID
        std::string oldFolderId = note->getParentFolderId();
        
        // If moving to the same folder, no operation needed
        if (oldFolderId == request.targetFolderId) {
            response.success = true;
            return response;
        }
        
        // Remove note from old folder
        auto oldFolder = folderRepository->findById(oldFolderId);
        if (oldFolder.has_value()) {
            Folder updatedOldFolder = oldFolder.value();
            updatedOldFolder.removeNoteId(request.noteId);
            folderRepository->update(updatedOldFolder);
        }
        
        // Add note to target folder
        Folder updatedTargetFolder = targetFolder.value();
        updatedTargetFolder.addNoteId(request.noteId);
        folderRepository->update(updatedTargetFolder);
        
        // Update note's parent folder ID
        Note updatedNote = note.value();
        updatedNote.setParentFolderId(request.targetFolderId);
        noteRepository->update(updatedNote);
        
        response.success = true;
        return response;
        
    } catch (const std::exception& e) {
        response.success = false;
        response.errorMessage = "Failed to move note: " + std::string(e.what());
        return response;
    }
}

std::string MoveNoteUseCase::validateRequest(const Request& request) {
    if (request.noteId.empty()) {
        return "Note ID cannot be empty";
    }
    
    if (request.targetFolderId.empty()) {
        return "Target folder ID cannot be empty";
    }
    
    // Validate that target folder exists
    if (!folderRepository->exists(request.targetFolderId)) {
        return "Target folder does not exist: " + request.targetFolderId;
    }
    
    return ""; // Valid
}