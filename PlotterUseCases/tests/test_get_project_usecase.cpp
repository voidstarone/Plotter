#include <gtest/gtest.h>
#include "usecases/GetProjectUseCase.h"
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class GetProjectUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> repository;
    std::unique_ptr<GetProjectUseCase> getUseCase;
    std::unique_ptr<CreateProjectUseCase> createUseCase;
    std::string testProjectId;

    void SetUp() override {
        repository = std::make_shared<InMemoryProjectRepository>();
        getUseCase = std::make_unique<GetProjectUseCase>(repository);
        createUseCase = std::make_unique<CreateProjectUseCase>(repository);

        // Create a test project
        CreateProjectUseCase::Request request;
        request.name = "Test Project";
        request.description = "For testing GetProject";
        auto response = createUseCase->execute(request);
        ASSERT_TRUE(response.isSuccess());
        testProjectId = response.getData().projectId;
    }

    void TearDown() override {
        repository.reset();
        getUseCase.reset();
        createUseCase.reset();
    }
};

TEST_F(GetProjectUseCaseTest, GetProject_ExistingProject_Success) {
    // Arrange
    GetProjectUseCase::Request request;
    request.projectId = testProjectId;

    // Act
    auto response = getUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_EQ(response.getData().project.id, testProjectId);
    EXPECT_EQ(response.getData().project.name, "Test Project");
    EXPECT_EQ(response.getData().project.description, "For testing GetProject");
}

TEST_F(GetProjectUseCaseTest, GetProject_NonExistentProject_NotFoundError) {
    // Arrange
    GetProjectUseCase::Request request;
    request.projectId = "nonexistent-id";

    // Act
    auto response = getUseCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::NOT_FOUND);
    EXPECT_NE(response.getErrorMessage().find("not found"), std::string::npos);
}

TEST_F(GetProjectUseCaseTest, GetProject_EmptyId_ValidationError) {
    // Arrange
    GetProjectUseCase::Request request;
    request.projectId = "";  // Invalid: empty ID

    // Act
    auto response = getUseCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(GetProjectUseCaseTest, GetProject_WithFolders) {
    // Arrange
    auto project = repository->findById(testProjectId);
    ASSERT_TRUE(project.has_value());

    // Add some folder IDs to the project
    project->addFolderId("folder-1");
    project->addFolderId("folder-2");
    repository->save(*project);

    GetProjectUseCase::Request request;
    request.projectId = testProjectId;

    // Act
    auto response = getUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_EQ(response.getData().project.folderIds.size(), 2);
}

TEST_F(GetProjectUseCaseTest, GetProject_WithProgressCallback) {
    // Arrange
    GetProjectUseCase::Request request;
    request.projectId = testProjectId;

    bool callbackCalled = false;
    request.progressCallback = [&](const UseCase::OperationProgress& progress) {
        callbackCalled = true;
        EXPECT_FALSE(progress.statusMessage.empty());
    };

    // Act
    auto response = getUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_TRUE(callbackCalled);
}

