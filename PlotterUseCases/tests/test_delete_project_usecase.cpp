#include <gtest/gtest.h>
#include "usecases/DeleteProjectUseCase.h"
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class DeleteProjectUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> repository;
    std::unique_ptr<DeleteProjectUseCase> deleteUseCase;
    std::unique_ptr<CreateProjectUseCase> createUseCase;
    std::string testProjectId;

    void SetUp() override {
        repository = std::make_shared<InMemoryProjectRepository>();
        deleteUseCase = std::make_unique<DeleteProjectUseCase>(repository);
        createUseCase = std::make_unique<CreateProjectUseCase>(repository);

        // Create a test project
        CreateProjectUseCase::Request request;
        request.name = "Project To Delete";
        request.description = "Will be deleted";
        auto response = createUseCase->execute(request);
        ASSERT_TRUE(response.isSuccess());
        testProjectId = response.getData().projectId;
    }

    void TearDown() override {
        repository.reset();
        deleteUseCase.reset();
        createUseCase.reset();
    }
};

TEST_F(DeleteProjectUseCaseTest, DeleteProject_ExistingProject_Success) {
    // Arrange
    DeleteProjectUseCase::Request request;
    request.projectId = testProjectId;

    // Verify project exists before deletion
    ASSERT_TRUE(repository->findById(testProjectId).has_value());

    // Act
    auto response = deleteUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_EQ(response.getData().deletedProjectId, testProjectId);

    // Verify project no longer exists
    EXPECT_FALSE(repository->findById(testProjectId).has_value());
}

TEST_F(DeleteProjectUseCaseTest, DeleteProject_NonExistentProject_NotFoundError) {
    // Arrange
    DeleteProjectUseCase::Request request;
    request.projectId = "nonexistent-id";

    // Act
    auto response = deleteUseCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::NOT_FOUND);
}

TEST_F(DeleteProjectUseCaseTest, DeleteProject_EmptyId_ValidationError) {
    // Arrange
    DeleteProjectUseCase::Request request;
    request.projectId = "";  // Invalid: empty ID

    // Act
    auto response = deleteUseCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
}

TEST_F(DeleteProjectUseCaseTest, DeleteProject_MultipleProjects_OnlyDeletesSpecified) {
    // Arrange - Create additional projects
    CreateProjectUseCase::Request createReq;
    createReq.name = "Project To Keep";
    createReq.description = "Should not be deleted";
    auto createResp = createUseCase->execute(createReq);
    ASSERT_TRUE(createResp.isSuccess());
    std::string keepProjectId = createResp.getData().projectId;

    DeleteProjectUseCase::Request deleteReq;
    deleteReq.projectId = testProjectId;

    // Act
    auto response = deleteUseCase->execute(deleteReq);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_FALSE(repository->findById(testProjectId).has_value());
    EXPECT_TRUE(repository->findById(keepProjectId).has_value());
}

