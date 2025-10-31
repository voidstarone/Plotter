#ifndef CREATENOTEUSECASE_H
#define CREATENOTEUSECASE_H

#include "repositories/NoteRepository.h"
#include "repositories/FolderRepository.h"
#include <memory>
#include <string>

/**
 * @brief Use case for creating a new note
 * 
 * This use case handles the business logic for creating a new note,
 * including validation, path generation, and linking to parent folder.
 */
class CreateNoteUseCase {
public:
    /**
     * @brief Request DTO for creating a note
     */
    struct Request {
        std::string name;
        std::string parentFolderId;
        std::string initialContent;  // Optional initial content
    };

    /**
     * @brief Response DTO for note creation
     */
    struct Response {
        std::string noteId;
        std::string notePath;
        bool success;
        std::string errorMessage;
    };

private:
    std::shared_ptr<NoteRepository> noteRepository;
    std::shared_ptr<FolderRepository> folderRepository;

public:
    /**
     * @brief Constructor
     * 
     * @param noteRepo The note repository to use for persistence
     * @param folderRepo The folder repository to validate parent folder
     */
    CreateNoteUseCase(
        std::shared_ptr<NoteRepository> noteRepo,
        std::shared_ptr<FolderRepository> folderRepo
    );
    
    /**
     * @brief Execute the use case
     * 
     * @param request The request containing note details
     * @return Response containing the result of the operation
     */
    Response execute(const Request& request);

private:
    /**
     * @brief Generate a unique ID for the note
     * 
     * @return A unique note ID string
     */
    std::string generateNoteId();
    
    /**
     * @brief Generate a storage path for the note
     * 
     * @param noteId The note ID
     * @param noteName The note name
     * @return A storage path string
     */
    std::string generateNotePath(const std::string& noteId, const std::string& noteName);
    
    /**
     * @brief Validate the request
     * 
     * @param request The request to validate
     * @return Error message if validation fails, empty string if valid
     */
    std::string validateRequest(const Request& request);
};

#endif // CREATENOTEUSECASE_H