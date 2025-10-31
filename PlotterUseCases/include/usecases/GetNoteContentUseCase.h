#ifndef GETNOTECONTENTUSECASE_H
#define GETNOTECONTENTUSECASE_H

#include "repositories/NoteRepository.h"
#include <memory>
#include <string>

/**
 * @brief Use case for retrieving note content
 * 
 * This use case handles the business logic for retrieving note content,
 * leveraging the lazy loading capabilities of the Note entity.
 */
class GetNoteContentUseCase {
public:
    /**
     * @brief Request DTO for getting note content
     */
    struct Request {
        std::string noteId;
    };

    /**
     * @brief Response DTO for note content retrieval
     */
    struct Response {
        std::string content;
        std::string noteName;
        std::string notePath;
        bool success;
        std::string errorMessage;
    };

private:
    std::shared_ptr<NoteRepository> noteRepository;

public:
    /**
     * @brief Constructor
     * 
     * @param noteRepo The note repository to use for retrieval
     */
    explicit GetNoteContentUseCase(std::shared_ptr<NoteRepository> noteRepo);
    
    /**
     * @brief Execute the use case
     * 
     * @param request The request containing note ID
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

#endif // GETNOTECONTENTUSECASE_H