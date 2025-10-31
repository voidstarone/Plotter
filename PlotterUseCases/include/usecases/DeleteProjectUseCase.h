#ifndef DELETEPROJECTUSECASE_H
#define DELETEPROJECTUSECASE_H

#include "repositories/ProjectRepository.h"
#include "repositories/FolderRepository.h"
#include "repositories/NoteRepository.h"
#include "BaseUseCase.h"
#include "UseCaseCommon.h"
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Use case for deleting a project and all its contents
 * 
 * This is a complex, long-running operation that:
 * - Can take significant time for large projects
 * - May fail partially (some items deleted, others not)
 * - Requires careful transaction-like behavior
 * - Benefits from progress reporting
 * - May need rollback on failure
 */
class DeleteProjectUseCase : public UseCase::BaseUseCase {
public:
    /**
     * @brief Request DTO for deleting a project
     */
    struct Request {
        std::string projectId;
        bool performRollbackOnFailure{true}; // Whether to rollback partial deletions
        UseCase::OperationConfig config; // Timeout and retry configuration
        UseCase::ProgressCallback progressCallback; // Progress reporting
    };

    /**
     * @brief Detailed results of deletion operation
     */
    struct DeleteProjectResult {
        std::string projectId;
        std::string projectName;
        int deletedFolders{0};
        int deletedNotes{0};
        int totalFolders{0};
        int totalNotes{0};
        bool wasPartialDeletion{false};
        std::vector<std::string> failedDeletions; // IDs of items that couldn't be deleted
        std::vector<std::string> rolledBackItems; // IDs of items that were rolled back
        std::chrono::system_clock::time_point completedAt;
        
        DeleteProjectResult() = default;
        DeleteProjectResult(const std::string& id, const std::string& name)
            : projectId(id), projectName(name), 
              completedAt(std::chrono::system_clock::now()) {}
    };

    /**
     * @brief Response DTO using enhanced error handling
     */
    using Response = UseCase::Response<DeleteProjectResult>;

private:
    std::shared_ptr<ProjectRepository> projectRepository;
    std::shared_ptr<FolderRepository> folderRepository;
    std::shared_ptr<NoteRepository> noteRepository;

public:
    /**
     * @brief Constructor
     * 
     * @param projectRepo The project repository
     * @param folderRepo The folder repository  
     * @param noteRepo The note repository
     * @param defaultConfig Default operation configuration
     */
    DeleteProjectUseCase(
        std::shared_ptr<ProjectRepository> projectRepo,
        std::shared_ptr<FolderRepository> folderRepo,
        std::shared_ptr<NoteRepository> noteRepo,
        const UseCase::OperationConfig& defaultConfig = UseCase::OperationConfig()
    );
    
    /**
     * @brief Execute the complex deletion operation
     * 
     * This method handles:
     * - Progressive deletion with detailed progress reporting
     * - Partial failure recovery (rollback capabilities)
     * - Timeout protection for large projects
     * - Detailed tracking of what was/wasn't deleted
     * 
     * @param request The request containing project ID and configuration
     * @return Response with detailed results or error information
     */
    Response execute(const Request& request);

private:
    /**
     * @brief Backup information for rollback
     */
    struct BackupInfo {
        std::string itemId;
        std::string itemType; // "project", "folder", "note"
        std::string parentId; // For restoring relationships
        // In a real system, you might store the full entity data
    };
    
    /**
     * @brief Validate the request
     * 
     * @param request The request to validate
     * @return Error message if validation fails, empty string if valid
     */
    std::string validateRequest(const Request& request);
    
    /**
     * @brief Count total items before deletion for progress reporting
     * 
     * @param projectId The project ID
     * @return Pair of (totalFolders, totalNotes)
     */
    std::pair<int, int> countProjectItems(const std::string& projectId);
    
    /**
     * @brief Recursively delete folder and track progress/failures
     * 
     * @param folderId The folder ID to delete
     * @param result Results object to update
     * @param backupInfo Backup information for potential rollback
     * @param progressCallback Progress reporting callback
     */
    void deleteFolderRecursively(
        const std::string& folderId, 
        DeleteProjectResult& result,
        std::vector<BackupInfo>& backupInfo,
        UseCase::ProgressCallback progressCallback
    );
    
    /**
     * @brief Attempt to rollback partially completed deletion
     * 
     * @param backupInfo The backup information collected during deletion
     * @param result Results object to update with rollback info
     */
    void performRollback(
        const std::vector<BackupInfo>& backupInfo,
        DeleteProjectResult& result
    );
};

#endif // DELETEPROJECTUSECASE_H