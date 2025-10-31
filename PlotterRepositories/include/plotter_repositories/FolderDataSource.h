#ifndef FOLDER_DATASOURCE_H
#define FOLDER_DATASOURCE_H

#include "DataSource.h"
#include "BaseDTOs.h"
#include <optional>
#include <vector>
#include <memory>

namespace plotter {
namespace repositories {

/**
 * @brief Abstract interface for Folder data sources
 * 
 * This interface defines the contract that any concrete datasource
 * (SQLite, Redis, FileSystem, etc.) must implement to provide
 * Folder data persistence using DTOs.
 * 
 * Data sources work with DTOs (inheriting from plotter::dto::FolderDTO).
 * The repository layer is responsible for converting between entities and DTOs.
 */
class FolderDataSource : public DataSource {
public:
    virtual ~FolderDataSource() = default;
    
    /**
     * @brief Save a folder DTO to the datasource
     * 
     * @param folderDTO The folder DTO to save
     * @return The ID of the saved folder
     * @throws std::runtime_error if the folder cannot be saved
     */
    virtual std::string save(const dto::FolderDTO& folderDTO) = 0;
    
    /**
     * @brief Find a folder by its ID
     * 
     * @param id The ID of the folder to find
     * @return An optional containing the folder DTO if found, std::nullopt otherwise
     */
    virtual std::optional<dto::FolderDTO*> findById(const std::string& id) = 0;
    
    /**
     * @brief Get all folders from the datasource
     * 
     * @return A vector containing all folder DTOs
     */
    virtual std::vector<dto::FolderDTO*> findAll() = 0;
    
    /**
     * @brief Find folders by parent project ID
     * 
     * @param projectId The ID of the parent project
     * @return A vector of folder DTOs belonging to the project
     */
    virtual std::vector<dto::FolderDTO*> findByProjectId(const std::string& projectId) = 0;
    
    /**
     * @brief Find folders by parent folder ID
     * 
     * @param parentFolderId The ID of the parent folder
     * @return A vector of child folder DTOs
     */
    virtual std::vector<dto::FolderDTO*> findByParentFolderId(const std::string& parentFolderId) = 0;
    
    /**
     * @brief Delete a folder by its ID
     * 
     * @param id The ID of the folder to delete
     * @return True if the folder was found and deleted, false otherwise
     */
    virtual bool deleteById(const std::string& id) = 0;
    
    /**
     * @brief Update an existing folder DTO
     * 
     * @param folderDTO The folder DTO with updated information
     * @return True if the folder was updated, false if not found
     */
    virtual bool update(const dto::FolderDTO& folderDTO) = 0;
    
    /**
     * @brief Check if a folder exists with the given ID
     * 
     * @param id The ID to check
     * @return True if a folder exists with that ID, false otherwise
     */
    virtual bool exists(const std::string& id) = 0;
    
    /**
     * @brief Clear all folders from the datasource
     * 
     * @return Number of folders cleared
     */
    virtual size_t clear() = 0;
};

} // namespace repositories
} // namespace plotter

#endif // FOLDER_DATASOURCE_H