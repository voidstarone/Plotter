#include <gtest/gtest.h>
#include "usecases/MoveFolderUseCase.h"
#include "usecases/CreateFolderUseCase.h"
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryFolderRepository.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class MoveFolderUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> projectRepo;
    std::shared_ptr<InMemoryFolderRepository> folderRepo;
    std::unique_ptr<MoveFolderUseCase> useCase;
    std::unique_ptr<CreateFolderUseCase> createFolderUseCase;
    std::unique_ptr<CreateProjectUseCase> createProjectUseCase;
    std::string testProjectId;
    std::string parentFolderId;
    std::string targetFolderId;
    std::string folderToMoveId;

    void SetUp() override {
        projectRepo = std::make_shared<InMemoryProjectRepository>();
        folderRepo = std::make_shared<InMemoryFolderRepository>();

        useCase = std::make_unique<MoveFolderUseCase>(folderRepo);
        createFolderUseCase = std::make_unique<CreateFolderUseCase>(projectRepo, folderRepo);
        createProjectUseCase = std::make_unique<CreateProjectUseCase>(projectRepo);

        // Create test project
        CreateProjectUseCase::Request projReq;
        projReq.name = "Move Folder Test Project";
        projReq.description = "For folder move tests";
        auto projResp = createProjectUseCase->execute(projReq);
        ASSERT_TRUE(projResp.isSuccess());
        testProjectId = projResp.getData().projectId;

        // Create parent folder
        CreateFolderUseCase::Request parentFolderReq;
        parentFolderReq.name = "Parent Folder";
        parentFolderReq.description = "Parent";
        parentFolderReq.parentProjectId = testProjectId;
        auto parentFolderResp = createFolderUseCase->execute(parentFolderReq);
        ASSERT_TRUE(parentFolderResp.isSuccess());
        parentFolderId = parentFolderResp.getData().folderId;

        // Create target folder
        CreateFolderUseCase::Request targetFolderReq;
        targetFolderReq.name = "Target Folder";
        targetFolderReq.description = "Target";
        targetFolderReq.parentProjectId = testProjectId;
        auto targetFolderResp = createFolderUseCase->execute(targetFolderReq);
        ASSERT_TRUE(targetFolderResp.isSuccess());
        targetFolderId = targetFolderResp.getData().folderId;

        // Create folder to move (child of parent folder)
        CreateFolderUseCase::Request folderToMoveReq;
        folderToMoveReq.name = "Folder To Move";
        folderToMoveReq.description = "This will be moved";
        folderToMoveReq.parentProjectId = testProjectId;
        folderToMoveReq.parentFolderId = parentFolderId;
        auto folderToMoveResp = createFolderUseCase->execute(folderToMoveReq);
        ASSERT_TRUE(folderToMoveResp.isSuccess());
        folderToMoveId = folderToMoveResp.getData().folderId;
    }

    void TearDown() override {
        projectRepo.reset();
        folderRepo.reset();
        useCase.reset();
        createFolderUseCase.reset();
        createProjectUseCase.reset();
    }
};

TEST_F(MoveFolderUseCaseTest, MoveFolder_ToNewParent_Success) {
    // Arrange
    MoveFolderUseCase::Request request;
    request.folderId = folderToMoveId;
    request.newParentFolderId = targetFolderId;

    // Verify folder is under parent folder
    auto folderBefore = folderRepo->findById(folderToMoveId);
    ASSERT_TRUE(folderBefore.has_value());
    EXPECT_EQ(folderBefore->getParentFolderId(), parentFolderId);

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess()) << "Error: " << response.getErrorMessage();
    EXPECT_EQ(response.getData().folderId, folderToMoveId);
    EXPECT_EQ(response.getData().newParentFolderId, targetFolderId);

    // Verify folder is now under target folder
    auto folderAfter = folderRepo->findById(folderToMoveId);
    ASSERT_TRUE(folderAfter.has_value());
    EXPECT_EQ(folderAfter->getParentFolderId(), targetFolderId);
}

TEST_F(MoveFolderUseCaseTest, MoveFolder_ToProjectRoot_Success) {
    // Arrange - Move to project root (no parent folder)
    MoveFolderUseCase::Request request;
    request.folderId = folderToMoveId;
    request.newParentFolderId = "";  // Empty = project root

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());

    // Verify folder is now at project root
    auto folder = folderRepo->findById(folderToMoveId);
    ASSERT_TRUE(folder.has_value());
    EXPECT_EQ(folder->getParentFolderId(), "");
}

TEST_F(MoveFolderUseCaseTest, MoveFolder_NonExistentFolder_NotFoundError) {
    // Arrange
    MoveFolderUseCase::Request request;
    request.folderId = "nonexistent-folder-id";
    request.newParentFolderId = targetFolderId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::NOT_FOUND);
}

TEST_F(MoveFolderUseCaseTest, MoveFolder_NonExistentTarget_ValidationError) {
    // Arrange
    MoveFolderUseCase::Request request;
    request.folderId = folderToMoveId;
    request.newParentFolderId = "nonexistent-target-id";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(MoveFolderUseCaseTest, MoveFolder_ToItself_ValidationError) {
    // Arrange - Try to move folder into itself
    MoveFolderUseCase::Request request;
    request.folderId = folderToMoveId;
    request.newParentFolderId = folderToMoveId;  // Same as folder being moved

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
    EXPECT_NE(response.getErrorMessage().find("cannot move"), std::string::npos);
}

TEST_F(MoveFolderUseCaseTest, MoveFolder_EmptyFolderId_ValidationError) {
    // Arrange
    MoveFolderUseCase::Request request;
    request.folderId = "";  // Invalid
    request.newParentFolderId = targetFolderId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(MoveFolderUseCaseTest, MoveFolder_PreservesFolderName) {
    // Arrange
    auto folderBefore = folderRepo->findById(folderToMoveId);
    ASSERT_TRUE(folderBefore.has_value());
    std::string originalName = folderBefore->getName();

    MoveFolderUseCase::Request request;
    request.folderId = folderToMoveId;
    request.newParentFolderId = targetFolderId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());

    auto folderAfter = folderRepo->findById(folderToMoveId);
    ASSERT_TRUE(folderAfter.has_value());
    EXPECT_EQ(folderAfter->getName(), originalName)
        << "Folder name should be preserved after move";
}
#include <gtest/gtest.h>
#include "usecases/GetNoteContentUseCase.h"
#include "usecases/CreateNoteUseCase.h"
#include "usecases/CreateFolderUseCase.h"
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryNoteRepository.h"
#include "InMemoryFolderRepository.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class GetNoteContentUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> projectRepo;
    std::shared_ptr<InMemoryFolderRepository> folderRepo;
    std::shared_ptr<InMemoryNoteRepository> noteRepo;
    std::unique_ptr<GetNoteContentUseCase> useCase;
    std::unique_ptr<CreateNoteUseCase> createNoteUseCase;
    std::unique_ptr<CreateFolderUseCase> createFolderUseCase;
    std::unique_ptr<CreateProjectUseCase> createProjectUseCase;
    std::string testProjectId;
    std::string testFolderId;
    std::string testNoteId;

    void SetUp() override {
        projectRepo = std::make_shared<InMemoryProjectRepository>();
        folderRepo = std::make_shared<InMemoryFolderRepository>();
        noteRepo = std::make_shared<InMemoryNoteRepository>();

        useCase = std::make_unique<GetNoteContentUseCase>(noteRepo);
        createNoteUseCase = std::make_unique<CreateNoteUseCase>(folderRepo, noteRepo);
        createFolderUseCase = std::make_unique<CreateFolderUseCase>(projectRepo, folderRepo);
        createProjectUseCase = std::make_unique<CreateProjectUseCase>(projectRepo);

        // Create test hierarchy
        CreateProjectUseCase::Request projReq;
        projReq.name = "Note Content Test Project";
        projReq.description = "For note content tests";
        auto projResp = createProjectUseCase->execute(projReq);
        ASSERT_TRUE(projResp.isSuccess());
        testProjectId = projResp.getData().projectId;

        CreateFolderUseCase::Request folderReq;
        folderReq.name = "Note Content Test Folder";
        folderReq.description = "For note content tests";
        folderReq.parentProjectId = testProjectId;
        auto folderResp = createFolderUseCase->execute(folderReq);
        ASSERT_TRUE(folderResp.isSuccess());
        testFolderId = folderResp.getData().folderId;

        CreateNoteUseCase::Request noteReq;
        noteReq.name = "Test Note";
        noteReq.parentFolderId = testFolderId;
        noteReq.initialContent = "This is the test note content";
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

TEST_F(GetNoteContentUseCaseTest, GetNoteContent_ExistingNote_Success) {
    // Arrange
    GetNoteContentUseCase::Request request;
    request.noteId = testNoteId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess()) << "Error: " << response.getErrorMessage();
    EXPECT_EQ(response.getData().noteId, testNoteId);
    EXPECT_EQ(response.getData().noteName, "Test Note");
    EXPECT_EQ(response.getData().content, "This is the test note content");
}

TEST_F(GetNoteContentUseCaseTest, GetNoteContent_NonExistentNote_NotFoundError) {
    // Arrange
    GetNoteContentUseCase::Request request;
    request.noteId = "nonexistent-note-id";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::NOT_FOUND);
}

TEST_F(GetNoteContentUseCaseTest, GetNoteContent_EmptyId_ValidationError) {
    // Arrange
    GetNoteContentUseCase::Request request;
    request.noteId = "";  // Invalid

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(GetNoteContentUseCaseTest, GetNoteContent_EmptyContent) {
    // Arrange - Create note with empty content
    CreateNoteUseCase::Request createReq;
    createReq.name = "Empty Note";
    createReq.parentFolderId = testFolderId;
    createReq.initialContent = "";
    auto createResp = createNoteUseCase->execute(createReq);
    ASSERT_TRUE(createResp.isSuccess());

    GetNoteContentUseCase::Request request;
    request.noteId = createResp.getData().noteId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_EQ(response.getData().content, "");
}

TEST_F(GetNoteContentUseCaseTest, GetNoteContent_LargeContent) {
    // Arrange - Create note with large content
    std::string largeContent(10000, 'X');  // 10KB of X's
    CreateNoteUseCase::Request createReq;
    createReq.name = "Large Note";
    createReq.parentFolderId = testFolderId;
    createReq.initialContent = largeContent;
    auto createResp = createNoteUseCase->execute(createReq);
    ASSERT_TRUE(createResp.isSuccess());

    GetNoteContentUseCase::Request request;
    request.noteId = createResp.getData().noteId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_EQ(response.getData().content.size(), 10000);
    EXPECT_EQ(response.getData().content, largeContent);
}

