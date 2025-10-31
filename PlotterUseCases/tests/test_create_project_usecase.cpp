#include <gtest/gtest.h>
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class CreateProjectUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> repository;
    std::unique_ptr<CreateProjectUseCase> useCase;

    void SetUp() override {
        repository = std::make_shared<InMemoryProjectRepository>();

        UseCase::OperationConfig config;
        config.timeoutMs = 5000;
        config.maxRetries = 3;
        config.retryDelayMs = 100;

        useCase = std::make_unique<CreateProjectUseCase>(repository, config);
    }

    void TearDown() override {
        repository.reset();
        useCase.reset();
    }
};

TEST_F(CreateProjectUseCaseTest, CreateProject_Success) {
    // Arrange
    CreateProjectUseCase::Request request;
    request.name = "Test Project";
    request.description = "A test project";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess()) << "Expected success but got error: "
                                      << response.getErrorMessage();
    EXPECT_FALSE(response.getData().projectId.empty());
    EXPECT_EQ(response.getData().projectName, "Test Project");

    // Verify project was saved in repository
    auto savedProject = repository->findById(response.getData().projectId);
    ASSERT_TRUE(savedProject.has_value());
    EXPECT_EQ(savedProject->getName(), "Test Project");
    EXPECT_EQ(savedProject->getDescription(), "A test project");
}

TEST_F(CreateProjectUseCaseTest, CreateProject_EmptyName_ValidationError) {
    // Arrange
    CreateProjectUseCase::Request request;
    request.name = "";  // Invalid: empty name
    request.description = "Test description";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
    EXPECT_FALSE(response.getErrorMessage().empty());

    // Verify no project was saved
    EXPECT_EQ(repository->findAll().size(), 0);
}

TEST_F(CreateProjectUseCaseTest, CreateProject_NameTooLong_ValidationError) {
    // Arrange
    CreateProjectUseCase::Request request;
    request.name = std::string(300, 'A');  // Invalid: 300 characters > 255 limit
    request.description = "Test description";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_FALSE(response.isSuccess());
    EXPECT_EQ(response.getErrorCategory(), UseCase::ErrorCategory::VALIDATION_ERROR);
    EXPECT_NE(response.getErrorMessage().find("255"), std::string::npos)
        << "Error message should mention the 255 character limit";
}

TEST_F(CreateProjectUseCaseTest, CreateProject_WithProgressCallback) {
    // Arrange
    CreateProjectUseCase::Request request;
    request.name = "Test Project with Progress";
    request.description = "Testing progress tracking";

    int progressCallCount = 0;
    double lastProgress = 0.0;

    request.progressCallback = [&](const UseCase::OperationProgress& progress) {
        progressCallCount++;
        lastProgress = progress.percentComplete;
        EXPECT_FALSE(progress.statusMessage.empty());
        EXPECT_GE(progress.currentStep, 0);
        EXPECT_LE(progress.currentStep, progress.totalSteps);
    };

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_GT(progressCallCount, 0) << "Progress callback should have been called";
    EXPECT_EQ(lastProgress, 100.0) << "Final progress should be 100%";
}

TEST_F(CreateProjectUseCaseTest, CreateProject_MultipleProjects) {
    // Arrange & Act
    std::vector<std::string> projectIds;

    for (int i = 0; i < 5; i++) {
        CreateProjectUseCase::Request request;
        request.name = "Project " + std::to_string(i);
        request.description = "Description " + std::to_string(i);

        auto response = useCase->execute(request);

        // Assert each creation
        ASSERT_TRUE(response.isSuccess()) << "Failed to create project " << i;
        projectIds.push_back(response.getData().projectId);
    }

    // Verify all projects exist
    auto allProjects = repository->findAll();
    EXPECT_EQ(allProjects.size(), 5);

    // Verify all IDs are unique
    std::set<std::string> uniqueIds(projectIds.begin(), projectIds.end());
    EXPECT_EQ(uniqueIds.size(), projectIds.size()) << "Project IDs should be unique";
}

TEST_F(CreateProjectUseCaseTest, CreateProject_WithoutDescription) {
    // Arrange
    CreateProjectUseCase::Request request;
    request.name = "Minimal Project";
    request.description = "";  // Empty description should be allowed

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    auto savedProject = repository->findById(response.getData().projectId);
    ASSERT_TRUE(savedProject.has_value());
    EXPECT_EQ(savedProject->getDescription(), "");
}

TEST_F(CreateProjectUseCaseTest, CreateProject_ExecutionTimeRecorded) {
    // Arrange
    CreateProjectUseCase::Request request;
    request.name = "Timed Project";
    request.description = "Test execution time tracking";

    // Act
    auto response = useCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_GT(response.getExecutionTimeMs(), 0) << "Execution time should be recorded";
}

