#include "usecases/GetNoteContentUseCase.h"
#include <stdexcept>

GetNoteContentUseCase::GetNoteContentUseCase(std::shared_ptr<NoteRepository> noteRepo)
    : noteRepository(noteRepo) {
    if (!noteRepository) {
        throw std::invalid_argument("NoteRepository cannot be null");
    }
}

GetNoteContentUseCase::Response GetNoteContentUseCase::execute(const Request& request) {
    Response response;
    
    // Validate request
    std::string validationError = validateRequest(request);
    if (!validationError.empty()) {
        response.success = false;
        response.errorMessage = validationError;
        return response;
    }
    
    try {
        // Retrieve note from repository
        auto note = noteRepository->findById(request.noteId);
        
        if (!note.has_value()) {
            response.success = false;
            response.errorMessage = "Note not found: " + request.noteId;
            return response;
        }
        
        // Get note metadata
        response.noteName = note->getName();
        response.notePath = note->getPath();
        
        // Get note content (this will trigger lazy loading)
        response.content = note->getContent();
        
        response.success = true;
        return response;
        
    } catch (const std::exception& e) {
        response.success = false;
        response.errorMessage = "Failed to retrieve note content: " + std::string(e.what());
        return response;
    }
}

std::string GetNoteContentUseCase::validateRequest(const Request& request) {
    if (request.noteId.empty()) {
        return "Note ID cannot be empty";
    }
    
    return ""; // Valid
}