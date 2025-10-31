#ifndef PROJECT_DATASOURCE_H
#define PROJECT_DATASOURCE_H

#include "DataSource.h"
#include "BaseDTOs.h"
#include <optional>
#include <vector>
#include <memory>

namespace plotter {
namespace repositories {

/**
 * @brief Abstract interface for Project data sources
 * 
 * This interface defines the contract that any concrete datasource
 * (SQLite, Redis, FileSystem, etc.) must implement to provide
 * Project data persistence using DTOs.
 * 
 * Data sources work with DTOs (inheriting from plotter::dto::ProjectDTO).
 * The repository layer is responsible for converting between entities and DTOs.
 * 
 * Implementations are responsible for:
 * - Handling connection/session management
 * - Error handling and retry logic
 * - Data validation
 */
class ProjectDataSource : public DataSource {
public:
    virtual ~ProjectDataSource() = default;
    
    /**
     * @brief Save a project DTO to the datasource
     * 
     * @param projectDTO The project DTO to save
     * @return The ID of the saved project
     * @throws std::runtime_error if the project cannot be saved
     */
    virtual std::string save(const dto::ProjectDTO& projectDTO) = 0;
    
    /**
     * @brief Find a project by its ID
     * 
     * @param id The ID of the project to find
     * @return An optional containing the project DTO if found, std::nullopt otherwise
     */
    virtual std::optional<dto::ProjectDTO*> findById(const std::string& id) = 0;
    
    /**
     * @brief Get all projects from the datasource
     * 
     * @return A vector containing all project DTOs
     */
    virtual std::vector<dto::ProjectDTO*> findAll() = 0;
    
    /**
     * @brief Delete a project by its ID
     * 
     * @param id The ID of the project to delete
     * @return True if the project was found and deleted, false otherwise
     */
    virtual bool deleteById(const std::string& id) = 0;
    
    /**
     * @brief Update an existing project DTO
     * 
     * @param projectDTO The project DTO with updated information
     * @return True if the project was updated, false if not found
     */
    virtual bool update(const dto::ProjectDTO& projectDTO) = 0;
    
    /**
     * @brief Check if a project exists with the given ID
     * 
     * @param id The ID to check
     * @return True if a project exists with that ID, false otherwise
     */
    virtual bool exists(const std::string& id) = 0;
    
    /**
     * @brief Clear all projects from the datasource (useful for cache/testing)
     * 
     * @return Number of projects cleared
     */
    virtual size_t clear() = 0;
};

} // namespace repositories
} // namespace plotter

#endif // PROJECT_DATASOURCE_H