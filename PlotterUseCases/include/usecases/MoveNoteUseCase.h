#ifndef MOVENOTEUSECASE_H
#define MOVENOTEUSECASE_H

#include "repositories/NoteRepository.h"
#include "repositories/FolderRepository.h"
#include "BaseUseCase.h"
#include "UseCaseCommon.h"
#include <memory>
#include <string>

/**
 * @brief Use case for moving a note to a different folder
 * 
 * This use case handles the business logic for moving a note from one
 * folder to another, updating all the necessary relationships.
 * 
 * This operation can fail in multiple ways:
 * - Source note might be locked by another operation
 * - Target folder might not exist or be locked
 * - Network/database issues during multi-step operation
 * - Partial completion (note removed from source but not added to target)
 */
class MoveNoteUseCase : public UseCase::BaseUseCase {
public:
    /**
     * @brief Request DTO for moving a note
     */
    struct Request {
        std::string noteId;
        std::string targetFolderId;
        bool performRollbackOnFailure{true}; // Rollback on partial failure
        UseCase::OperationConfig config; // Timeout and retry configuration
        UseCase::ProgressCallback progressCallback; // Progress reporting
    };

    /**
     * @brief Result data for successful note move
     */
    struct MoveNoteResult {
        std::string noteId;
        std::string noteName;
        std::string sourceFolderId;
        std::string targetFolderId;
        std::chrono::system_clock::time_point movedAt;
        bool requiredRollback{false}; // Whether rollback was performed
        
        MoveNoteResult() = default;
        MoveNoteResult(const std::string& id, const std::string& name,
                      const std::string& source, const std::string& target)
            : noteId(id), noteName(name), sourceFolderId(source), 
              targetFolderId(target), movedAt(std::chrono::system_clock::now()) {}
    };

    /**
     * @brief Response DTO using enhanced error handling
     */
    using Response = UseCase::Response<MoveNoteResult>;

private:
    std::shared_ptr<NoteRepository> noteRepository;
    std::shared_ptr<FolderRepository> folderRepository;

public:
    /**
     * @brief Constructor
     * 
     * @param noteRepo The note repository
     * @param folderRepo The folder repository
     */
    MoveNoteUseCase(
        std::shared_ptr<NoteRepository> noteRepo,
        std::shared_ptr<FolderRepository> folderRepo
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

#endif // MOVENOTEUSECASE_H