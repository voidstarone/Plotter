#include <gtest/gtest.h>
#include "usecases/MoveNoteUseCase.h"
#include "usecases/CreateNoteUseCase.h"
#include "usecases/CreateFolderUseCase.h"
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryNoteRepository.h"
#include "InMemoryFolderRepository.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class MoveNoteUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> projectRepo;
    std::shared_ptr<InMemoryFolderRepository> folderRepo;
    std::shared_ptr<InMemoryNoteRepository> noteRepo;
    std::unique_ptr<MoveNoteUseCase> useCase;
    std::unique_ptr<CreateNoteUseCase> createNoteUseCase;
    std::unique_ptr<CreateFolderUseCase> createFolderUseCase;
    std::unique_ptr<CreateProjectUseCase> createProjectUseCase;
    std::string testProjectId;
    std::string sourceFolderId;
    std::string targetFolderId;
    std::string testNoteId;

    void SetUp() override {
        projectRepo = std::make_shared<InMemoryProjectRepository>();
        folderRepo = std::make_shared<InMemoryFolderRepository>();
        noteRepo = std::make_shared<InMemoryNoteRepository>();

        useCase = std::make_unique<MoveNoteUseCase>(folderRepo, noteRepo);
        createNoteUseCase = std::make_unique<CreateNoteUseCase>(folderRepo, noteRepo);
        createFolderUseCase = std::make_unique<CreateFolderUseCase>(projectRepo, folderRepo);
        createProjectUseCase = std::make_unique<CreateProjectUseCase>(projectRepo);

        // Create test project
        CreateProjectUseCase::Request projReq;
        projReq.name = "Move Note Test Project";
        projReq.description = "For note move tests";
        auto projResp = createProjectUseCase->execute(projReq);
        ASSERT_TRUE(projResp.isSuccess());
        testProjectId = projResp.getData().projectId;

        // Create source folder
        CreateFolderUseCase::Request sourceFolderReq;
        sourceFolderReq.name = "Source Folder";
        sourceFolderReq.description = "Source";
        sourceFolderReq.parentProjectId = testProjectId;
        auto sourceFolderResp = createFolderUseCase->execute(sourceFolderReq);
        ASSERT_TRUE(sourceFolderResp.isSuccess());
        sourceFolderId = sourceFolderResp.getData().folderId;

        // Create target folder
        CreateFolderUseCase::Request targetFolderReq;
        targetFolderReq.name = "Target Folder";
        targetFolderReq.description = "Target";
        targetFolderReq.parentProjectId = testProjectId;
        auto targetFolderResp = createFolderUseCase->execute(targetFolderReq);
        ASSERT_TRUE(targetFolderResp.isSuccess());
        targetFolderId = targetFolderResp.getData().folderId;

        // Create test note in source folder
        CreateNoteUseCase::Request noteReq;
        noteReq.name = "Note To Move";
        noteReq.parentFolderId = sourceFolderId;
        noteReq.initialContent = "This note will be moved";
        auto noteResp = createNoteUseCase->execute(noteReq);
        ASSERT_TRUE(noteResp.isSuccess());
        testNoteId = noteResp.getData().noteId;
    }

    void TearDown() override {
        projectRepo.reset();
        folderRepo.reset();
        noteRepo.reset();
        useCase.reset();
        createNoteUseCase.reset();
        createFolderUseCase.reset();
        createProjectUseCase.reset();
    }
};

TEST_F(MoveNoteUseCaseTest, MoveNote_ToNewFolder_Success) {
    // Arrange
    MoveNoteUseCase::Request request;
    request.noteId = testNoteId;
    request.targetFolderId = targetFolderId;

    // Verify note is in source folder
    auto noteBefore = noteRepo->findById(testNoteId);
    ASSERT_TRUE(noteBefore.has_value());
    EXPECT_EQ(noteBefore->getParentFolderId(), sourceFolderId);

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess()) << "Error: " << response.getErrorMessage();
    EXPECT_EQ(response.getData().noteId, testNoteId);
    EXPECT_EQ(response.getData().newFolderId, targetFolderId);

    // Verify note is now in target folder
    auto noteAfter = noteRepo->findById(testNoteId);
    ASSERT_TRUE(noteAfter.has_value());
    EXPECT_EQ(noteAfter->getParentFolderId(), targetFolderId);
}

TEST_F(MoveNoteUseCaseTest, MoveNote_NonExistentNote_NotFoundError) {
    // Arrange
    MoveNoteUseCase::Request request;
    request.noteId = "nonexistent-note-id";
    request.targetFolderId = targetFolderId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::NOT_FOUND);
}

TEST_F(MoveNoteUseCaseTest, MoveNote_NonExistentTargetFolder_ValidationError) {
    // Arrange
    MoveNoteUseCase::Request request;
    request.noteId = testNoteId;
    request.targetFolderId = "nonexistent-folder-id";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(MoveNoteUseCaseTest, MoveNote_ToSameFolder_Success) {
    // Arrange - Move to same folder should succeed (no-op)
    MoveNoteUseCase::Request request;
    request.noteId = testNoteId;
    request.targetFolderId = sourceFolderId;  // Same as current

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());

    // Verify note is still in source folder
    auto note = noteRepo->findById(testNoteId);
    ASSERT_TRUE(note.has_value());
    EXPECT_EQ(note->getParentFolderId(), sourceFolderId);
}

TEST_F(MoveNoteUseCaseTest, MoveNote_EmptyNoteId_ValidationError) {
    // Arrange
    MoveNoteUseCase::Request request;
    request.noteId = "";  // Invalid
    request.targetFolderId = targetFolderId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(MoveNoteUseCaseTest, MoveNote_PreservesNoteContent) {
    // Arrange
    auto noteBefore = noteRepo->findById(testNoteId);
    ASSERT_TRUE(noteBefore.has_value());
    std::string originalContent = noteBefore->getContent();

    MoveNoteUseCase::Request request;
    request.noteId = testNoteId;
    request.targetFolderId = targetFolderId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());

    auto noteAfter = noteRepo->findById(testNoteId);
    ASSERT_TRUE(noteAfter.has_value());
    EXPECT_EQ(noteAfter->getContent(), originalContent)
        << "Note content should be preserved after move";
}

