#include <gtest/gtest.h>
#include "usecases/CreateFolderUseCase.h"
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryFolderRepository.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class CreateFolderUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> projectRepo;
    std::shared_ptr<InMemoryFolderRepository> folderRepo;
    std::unique_ptr<CreateFolderUseCase> useCase;
    std::unique_ptr<CreateProjectUseCase> createProjectUseCase;
    std::string testProjectId;

    void SetUp() override {
        projectRepo = std::make_shared<InMemoryProjectRepository>();
        folderRepo = std::make_shared<InMemoryFolderRepository>();
        useCase = std::make_unique<CreateFolderUseCase>(projectRepo, folderRepo);
        createProjectUseCase = std::make_unique<CreateProjectUseCase>(projectRepo);

        // Create a test project
        CreateProjectUseCase::Request request;
        request.name = "Parent Project";
        request.description = "For folder tests";
        auto response = createProjectUseCase->execute(request);
        ASSERT_TRUE(response.isSuccess());
        testProjectId = response.getData().projectId;
    }

    void TearDown() override {
        projectRepo.reset();
        folderRepo.reset();
        useCase.reset();
        createProjectUseCase.reset();
    }
};

TEST_F(CreateFolderUseCaseTest, CreateFolder_TopLevel_Success) {
    // Arrange
    CreateFolderUseCase::Request request;
    request.name = "Top Level Folder";
    request.description = "A folder at project root";
    request.parentProjectId = testProjectId;
    request.parentFolderId = "";  // No parent folder = top level

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess()) << "Error: " << response.getErrorMessage();
    EXPECT_FALSE(response.getData().folderId.empty());
    EXPECT_EQ(response.getData().folderName, "Top Level Folder");

    // Verify folder was saved
    auto savedFolder = folderRepo->findById(response.getData().folderId);
    ASSERT_TRUE(savedFolder.has_value());
    EXPECT_EQ(savedFolder->getName(), "Top Level Folder");
    EXPECT_EQ(savedFolder->getParentProjectId(), testProjectId);
}

TEST_F(CreateFolderUseCaseTest, CreateFolder_Nested_Success) {
    // Arrange - Create parent folder first
    CreateFolderUseCase::Request parentRequest;
    parentRequest.name = "Parent Folder";
    parentRequest.description = "Parent";
    parentRequest.parentProjectId = testProjectId;
    parentRequest.parentFolderId = "";

    auto parentResponse = useCase->execute(parentRequest);
    ASSERT_TRUE(parentResponse.isSuccess());
    std::string parentFolderId = parentResponse.getData().folderId;

    // Create nested folder
    CreateFolderUseCase::Request request;
    request.name = "Nested Folder";
    request.description = "Inside parent";
    request.parentProjectId = testProjectId;
    request.parentFolderId = parentFolderId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    auto savedFolder = folderRepo->findById(response.getData().folderId);
    ASSERT_TRUE(savedFolder.has_value());
    EXPECT_EQ(savedFolder->getParentFolderId(), parentFolderId);
}

TEST_F(CreateFolderUseCaseTest, CreateFolder_EmptyName_ValidationError) {
    // Arrange
    CreateFolderUseCase::Request request;
    request.name = "";  // Invalid
    request.description = "Test";
    request.parentProjectId = testProjectId;

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(CreateFolderUseCaseTest, CreateFolder_NonExistentProject_ValidationError) {
    // Arrange
    CreateFolderUseCase::Request request;
    request.name = "Orphan Folder";
    request.description = "No parent project";
    request.parentProjectId = "nonexistent-project-id";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(CreateFolderUseCaseTest, CreateFolder_MultipleFoldersInProject) {
    // Arrange & Act - Create multiple folders
    std::vector<std::string> folderIds;

    for (int i = 0; i < 5; i++) {
        CreateFolderUseCase::Request request;
        request.name = "Folder " + std::to_string(i);
        request.description = "Folder number " + std::to_string(i);
        request.parentProjectId = testProjectId;

        auto response = useCase->execute(request);
        ASSERT_TRUE(response.isSuccess());
        folderIds.push_back(response.getData().folderId);
    }

    // Assert
    EXPECT_EQ(folderIds.size(), 5);

    // Verify all IDs are unique
    std::set<std::string> uniqueIds(folderIds.begin(), folderIds.end());
    EXPECT_EQ(uniqueIds.size(), folderIds.size());
}

