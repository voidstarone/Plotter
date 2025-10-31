#ifndef PROJECTREPOSITORY_H
#define PROJECTREPOSITORY_H

#include "Project.h"
#include <memory>
#include <optional>
#include <vector>

/**
 * @brief Abstract repository interface for Project entities
 * 
 * This interface defines the contract for persisting and retrieving Project entities.
 * Concrete implementations can use different storage backends (in-memory, database, etc.)
 * while maintaining the same interface for use cases.
 */
class ProjectRepository {
public:
    virtual ~ProjectRepository() = default;
    
    /**
     * @brief Save a project to the repository
     * 
     * @param project The project to save
     * @return The ID of the saved project
     * @throws std::runtime_error if the project cannot be saved
     */
    virtual std::string save(const Project& project) = 0;
    
    /**
     * @brief Find a project by its ID
     * 
     * @param id The ID of the project to find
     * @return An optional containing the project if found, std::nullopt otherwise
     */
    virtual std::optional<Project> findById(const std::string& id) = 0;
    
    /**
     * @brief Get all projects in the repository
     * 
     * @return A vector containing all projects
     */
    virtual std::vector<Project> findAll() = 0;
    
    /**
     * @brief Delete a project by its ID
     * 
     * @param id The ID of the project to delete
     * @return True if the project was found and deleted, false otherwise
     */
    virtual bool deleteById(const std::string& id) = 0;
    
    /**
     * @brief Update an existing project
     * 
     * @param project The project with updated information
     * @throws std::runtime_error if the project cannot be updated
     */
    virtual void update(const Project& project) = 0;
    
    /**
     * @brief Check if a project exists with the given ID
     * 
     * @param id The ID to check
     * @return True if a project exists with that ID, false otherwise
     */
    virtual bool exists(const std::string& id) = 0;
};

#endif // PROJECTREPOSITORY_H