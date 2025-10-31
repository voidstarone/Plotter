#include <gtest/gtest.h>
#include "usecases/CreateNoteUseCase.h"
#include "usecases/CreateFolderUseCase.h"
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryNoteRepository.h"
#include "InMemoryFolderRepository.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class CreateNoteUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> projectRepo;
    std::shared_ptr<InMemoryFolderRepository> folderRepo;
    std::shared_ptr<InMemoryNoteRepository> noteRepo;
    std::unique_ptr<CreateNoteUseCase> useCase;
    std::unique_ptr<CreateFolderUseCase> createFolderUseCase;
    std::unique_ptr<CreateProjectUseCase> createProjectUseCase;
    std::string testProjectId;
    std::string testFolderId;

    void SetUp() override {
        projectRepo = std::make_shared<InMemoryProjectRepository>();
        folderRepo = std::make_shared<InMemoryFolderRepository>();
        noteRepo = std::make_shared<InMemoryNoteRepository>();

        useCase = std::make_unique<CreateNoteUseCase>(folderRepo, noteRepo);
        createFolderUseCase = std::make_unique<CreateFolderUseCase>(projectRepo, folderRepo);
        createProjectUseCase = std::make_unique<CreateProjectUseCase>(projectRepo);

        // Create test project
        CreateProjectUseCase::Request projReq;
        projReq.name = "Note Test Project";
        projReq.description = "For note tests";
        auto projResp = createProjectUseCase->execute(projReq);
        ASSERT_TRUE(projResp.isSuccess());
        testProjectId = projResp.getData().projectId;

        // Create test folder
        CreateFolderUseCase::Request folderReq;
        folderReq.name = "Note Test Folder";
        folderReq.description = "For note tests";
        folderReq.parentProjectId = testProjectId;
        auto folderResp = createFolderUseCase->execute(folderReq);
        ASSERT_TRUE(folderResp.isSuccess());
        testFolderId = folderResp.getData().folderId;
    }

    void TearDown() override {
        projectRepo.reset();
        folderRepo.reset();
        noteRepo.reset();
        useCase.reset();
        createFolderUseCase.reset();
        createProjectUseCase.reset();
    }
};

TEST_F(CreateNoteUseCaseTest, CreateNote_Success) {
    // Arrange
    CreateNoteUseCase::Request request;
    request.name = "Test Note";
    request.parentFolderId = testFolderId;
    request.initialContent = "This is the initial content";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess()) << "Error: " << response.getErrorMessage();
    EXPECT_FALSE(response.getData().noteId.empty());
    EXPECT_EQ(response.getData().noteName, "Test Note");

    // Verify note was saved
    auto savedNote = noteRepo->findById(response.getData().noteId);
    ASSERT_TRUE(savedNote.has_value());
    EXPECT_EQ(savedNote->getName(), "Test Note");
    EXPECT_EQ(savedNote->getParentFolderId(), testFolderId);
}

TEST_F(CreateNoteUseCaseTest, CreateNote_EmptyName_ValidationError) {
    // Arrange
    CreateNoteUseCase::Request request;
    request.name = "";  // Invalid
    request.parentFolderId = testFolderId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(CreateNoteUseCaseTest, CreateNote_NonExistentFolder_ValidationError) {
    // Arrange
    CreateNoteUseCase::Request request;
    request.name = "Orphan Note";
    request.parentFolderId = "nonexistent-folder-id";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(CreateNoteUseCaseTest, CreateNote_WithoutInitialContent) {
    // Arrange
    CreateNoteUseCase::Request request;
    request.name = "Empty Note";
    request.parentFolderId = testFolderId;
    request.initialContent = "";  // Empty content should be allowed

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
}

TEST_F(CreateNoteUseCaseTest, CreateNote_MultipleNotesInFolder) {
    // Arrange & Act
    std::vector<std::string> noteIds;

    for (int i = 0; i < 10; i++) {
        CreateNoteUseCase::Request request;
        request.name = "Note " + std::to_string(i);
        request.parentFolderId = testFolderId;
        request.initialContent = "Content for note " + std::to_string(i);

        auto response = useCase->execute(request);
        ASSERT_TRUE(response.isSuccess());
        noteIds.push_back(response.getData().noteId);
    }

    // Assert
    EXPECT_EQ(noteIds.size(), 10);

    // Verify all IDs are unique
    std::set<std::string> uniqueIds(noteIds.begin(), noteIds.end());
    EXPECT_EQ(uniqueIds.size(), noteIds.size());
}

TEST_F(CreateNoteUseCaseTest, CreateNote_WithProgressCallback) {
    // Arrange
    CreateNoteUseCase::Request request;
    request.name = "Tracked Note";
    request.parentFolderId = testFolderId;

    bool callbackCalled = false;
    request.progressCallback = [&](const UseCase::OperationProgress& progress) {
        callbackCalled = true;
        EXPECT_FALSE(progress.statusMessage.empty());
    };

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_TRUE(callbackCalled);
}
cmake_minimum_required(VERSION 3.16)

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/tests/mocks)
include_directories(${CMAKE_SOURCE_DIR}/../PlotterEntities/include)

# Mock repositories library
add_library(UseCaseMocks
    mocks/InMemoryProjectRepository.cpp
    mocks/InMemoryFolderRepository.cpp
    mocks/InMemoryNoteRepository.cpp
)

target_link_libraries(UseCaseMocks
    NoteTaker
)

# Test executable
add_executable(UseCaseTests
    test_create_project_usecase.cpp
    test_list_projects_usecase.cpp
    test_get_project_usecase.cpp
    test_delete_project_usecase.cpp
    test_create_folder_usecase.cpp
    test_create_note_usecase.cpp
    test_get_note_content_usecase.cpp
    test_move_note_usecase.cpp
    test_move_folder_usecase.cpp
)

target_link_libraries(UseCaseTests
    PlotterUseCases
    UseCaseMocks
    NoteTaker
    ${GTEST_LIBRARIES}
    pthread
)

# Register tests with CTest
add_test(NAME UseCaseTests COMMAND UseCaseTests)

