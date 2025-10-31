#include <gtest/gtest.h>
#include "usecases/ListProjectsUseCase.h"
#include "usecases/CreateProjectUseCase.h"
#include "InMemoryProjectRepository.h"
#include <memory>

class ListProjectsUseCaseTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryProjectRepository> repository;
    std::unique_ptr<ListProjectsUseCase> listUseCase;
    std::unique_ptr<CreateProjectUseCase> createUseCase;

    void SetUp() override {
        repository = std::make_shared<InMemoryProjectRepository>();
        listUseCase = std::make_unique<ListProjectsUseCase>(repository);
        createUseCase = std::make_unique<CreateProjectUseCase>(repository);
    }

    void TearDown() override {
        repository.reset();
        listUseCase.reset();
        createUseCase.reset();
    }

    void createTestProject(const std::string& name, const std::string& description) {
        CreateProjectUseCase::Request request;
        request.name = name;
        request.description = description;
        auto response = createUseCase->execute(request);
        ASSERT_TRUE(response.isSuccess()) << "Failed to create test project";
    }
};

TEST_F(ListProjectsUseCaseTest, ListProjects_EmptyRepository) {
    // Arrange
    ListProjectsUseCase::Request request;

    // Act
    auto response = listUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_EQ(response.getData().projects.size(), 0);
    EXPECT_EQ(response.getData().totalCount, 0);
}

TEST_F(ListProjectsUseCaseTest, ListProjects_SingleProject) {
    // Arrange
    createTestProject("Single Project", "The only project");
    ListProjectsUseCase::Request request;

    // Act
    auto response = listUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    ASSERT_EQ(response.getData().projects.size(), 1);
    EXPECT_EQ(response.getData().projects[0].name, "Single Project");
    EXPECT_EQ(response.getData().projects[0].description, "The only project");
    EXPECT_EQ(response.getData().totalCount, 1);
}

TEST_F(ListProjectsUseCaseTest, ListProjects_MultipleProjects) {
    // Arrange
    createTestProject("Project Alpha", "First project");
    createTestProject("Project Beta", "Second project");
    createTestProject("Project Gamma", "Third project");

    ListProjectsUseCase::Request request;

    // Act
    auto response = listUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_EQ(response.getData().projects.size(), 3);
    EXPECT_EQ(response.getData().totalCount, 3);

    // Verify all projects are present
    std::set<std::string> names;
    for (const auto& proj : response.getData().projects) {
        names.insert(proj.name);
    }
    EXPECT_EQ(names.count("Project Alpha"), 1);
    EXPECT_EQ(names.count("Project Beta"), 1);
    EXPECT_EQ(names.count("Project Gamma"), 1);
}

TEST_F(ListProjectsUseCaseTest, ListProjects_WithProgressCallback) {
    // Arrange
    createTestProject("Project 1", "Description 1");
    createTestProject("Project 2", "Description 2");

    ListProjectsUseCase::Request request;

    int progressCallCount = 0;
    request.progressCallback = [&](const UseCase::OperationProgress& progress) {
        progressCallCount++;
        EXPECT_FALSE(progress.statusMessage.empty());
    };

    // Act
    auto response = listUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_GT(progressCallCount, 0) << "Progress callback should have been called";
}

TEST_F(ListProjectsUseCaseTest, ListProjects_ProjectsHaveValidData) {
    // Arrange
    createTestProject("Data Test Project", "Testing data integrity");
    ListProjectsUseCase::Request request;

    // Act
    auto response = listUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    ASSERT_EQ(response.getData().projects.size(), 1);

    const auto& project = response.getData().projects[0];
    EXPECT_FALSE(project.id.empty()) << "Project ID should not be empty";
    EXPECT_FALSE(project.name.empty()) << "Project name should not be empty";
    EXPECT_EQ(project.name, "Data Test Project");
    EXPECT_EQ(project.description, "Testing data integrity");
}

TEST_F(ListProjectsUseCaseTest, ListProjects_LargeNumberOfProjects) {
    // Arrange - Create many projects
    const int projectCount = 50;
    for (int i = 0; i < projectCount; i++) {
        createTestProject("Project " + std::to_string(i), "Description " + std::to_string(i));
    }

    ListProjectsUseCase::Request request;

    // Act
    auto response = listUseCase->execute(request);

    // Assert
    ASSERT_TRUE(response.isSuccess());
    EXPECT_EQ(response.getData().projects.size(), projectCount);
    EXPECT_EQ(response.getData().totalCount, projectCount);
}

