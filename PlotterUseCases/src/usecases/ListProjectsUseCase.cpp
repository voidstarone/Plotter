#include "usecases/ListProjectsUseCase.h"
#include <stdexcept>

ListProjectsUseCase::ListProjectsUseCase(std::shared_ptr<ProjectRepository> repo)
    : projectRepository(repo) {
    if (!projectRepository) {
        throw std::invalid_argument("ProjectRepository cannot be null");
    }
}

ListProjectsUseCase::Response ListProjectsUseCase::execute(const Request& request) {
    Response response;
    
    try {
        // Retrieve all projects from repository
        response.projects = projectRepository->findAll();
        response.success = true;
        return response;
        
    } catch (const std::exception& e) {
        response.success = false;
        response.errorMessage = "Failed to list projects: " + std::string(e.what());
        return response;
    }
}