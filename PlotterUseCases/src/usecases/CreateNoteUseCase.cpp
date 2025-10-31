#include "usecases/CreateNoteUseCase.h"
#include "Note.h"
#include <chrono>
#include <stdexcept>
#include <algorithm>

CreateNoteUseCase::CreateNoteUseCase(
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

CreateNoteUseCase::Response CreateNoteUseCase::execute(const Request& request) {
    Response response;
    
    // Validate request
    std::string validationError = validateRequest(request);
    if (!validationError.empty()) {
        response.success = false;
        response.errorMessage = validationError;
        return response;
    }
    
    try {
        // Generate unique ID and path
        std::string id = generateNoteId();
        std::string path = generateNotePath(id, request.name);
        response.noteId = id;
        response.notePath = path;
        
        // Create entity
        Note note(id, request.name, path, request.parentFolderId);
        
        // Set initial content if provided
        if (!request.initialContent.empty()) {
            note.setContent(request.initialContent);
        }
        
        // Persist via repository
        std::string savedId = noteRepository->save(note);
        
        // Update parent folder to include this new note
        auto parentFolder = folderRepository->findById(request.parentFolderId);
        if (parentFolder.has_value()) {
            Folder updatedFolder = parentFolder.value();
            updatedFolder.addNoteId(savedId);
            folderRepository->update(updatedFolder);
        }
        
        response.success = true;
        return response;
        
    } catch (const std::exception& e) {
        response.success = false;
        response.errorMessage = "Failed to create note: " + std::string(e.what());
        return response;
    }
}

std::string CreateNoteUseCase::generateNoteId() {
    // In a real system, you'd use a UUID library or proper ID generation service
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return "note_" + std::to_string(millis);
}

std::string CreateNoteUseCase::generateNotePath(const std::string& noteId, const std::string& noteName) {
    // Create a filesystem-safe version of the note name
    std::string safeName = noteName;
    
    // Replace problematic characters with underscores
    std::replace_if(safeName.begin(), safeName.end(), 
                    [](char c) { return !std::isalnum(c) && c != '-' && c != '_'; }, '_');
    
    // Limit length
    if (safeName.length() > 50) {
        safeName = safeName.substr(0, 50);
    }
    
    // Combine ID and safe name for uniqueness
    return "notes/" + noteId + "_" + safeName + ".txt";
}

std::string CreateNoteUseCase::validateRequest(const Request& request) {
    if (request.name.empty()) {
        return "Note name cannot be empty";
    }
    
    if (request.name.length() > 255) {
        return "Note name cannot exceed 255 characters";
    }
    
    if (request.parentFolderId.empty()) {
        return "Parent folder ID cannot be empty";
    }
    
    // Validate that parent folder exists
    if (!folderRepository->exists(request.parentFolderId)) {
        return "Parent folder does not exist: " + request.parentFolderId;
    }
    
    return ""; // Valid
}