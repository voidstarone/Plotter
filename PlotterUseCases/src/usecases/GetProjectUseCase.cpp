#include "usecases/GetProjectUseCase.h"
#include <stdexcept>

GetProjectUseCase::GetProjectUseCase(std::shared_ptr<ProjectRepository> repo)
    : projectRepository(repo) {
    if (!projectRepository) {
        throw std::invalid_argument("ProjectRepository cannot be null");
    }
}

GetProjectUseCase::Response GetProjectUseCase::execute(const Request& request) {
    Response response;
    
    // Validate request
    std::string validationError = validateRequest(request);
    if (!validationError.empty()) {
        response.success = false;
        response.errorMessage = validationError;
        return response;
    }
    
    try {
        // Retrieve project from repository
        response.project = projectRepository->findById(request.projectId);
        
        if (!response.project.has_value()) {
            response.success = false;
            response.errorMessage = "Project not found: " + request.projectId;
        } else {
            response.success = true;
        }
        
        return response;
        
    } catch (const std::exception& e) {
        response.success = false;
        response.errorMessage = "Failed to retrieve project: " + std::string(e.what());
        return response;
    }
}

std::string GetProjectUseCase::validateRequest(const Request& request) {
    if (request.projectId.empty()) {
        return "Project ID cannot be empty";
    }
    
    return ""; // Valid
}