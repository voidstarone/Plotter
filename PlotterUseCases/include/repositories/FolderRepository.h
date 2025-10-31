#ifndef FOLDERREPOSITORY_H
#define FOLDERREPOSITORY_H

#include "Folder.h"
#include <memory>
#include <optional>
#include <vector>

/**
 * @brief Abstract repository interface for Folder entities
 * 
 * This interface defines the contract for persisting and retrieving Folder entities.
 * Concrete implementations can use different storage backends (in-memory, database, etc.)
 * while maintaining the same interface for use cases.
 */
class FolderRepository {
public:
    virtual ~FolderRepository() = default;
    
    /**
     * @brief Save a folder to the repository
     * 
     * @param folder The folder to save
     * @return The ID of the saved folder
     * @throws std::runtime_error if the folder cannot be saved
     */
    virtual std::string save(const Folder& folder) = 0;
    
    /**
     * @brief Find a folder by its ID
     * 
     * @param id The ID of the folder to find
     * @return An optional containing the folder if found, std::nullopt otherwise
     */
    virtual std::optional<Folder> findById(const std::string& id) = 0;
    
    /**
     * @brief Get all folders in the repository
     * 
     * @return A vector containing all folders
     */
    virtual std::vector<Folder> findAll() = 0;
    
    /**
     * @brief Find folders by parent project ID
     * 
     * @param parentProjectId The ID of the parent project
     * @return A vector containing folders that belong to the specified project
     */
    virtual std::vector<Folder> findByParentProjectId(const std::string& parentProjectId) = 0;
    
    /**
     * @brief Find folders by parent folder ID
     * 
     * @param parentFolderId The ID of the parent folder
     * @return A vector containing subfolders of the specified folder
     */
    virtual std::vector<Folder> findByParentFolderId(const std::string& parentFolderId) = 0;
    
    /**
     * @brief Delete a folder by its ID
     * 
     * @param id The ID of the folder to delete
     * @return True if the folder was found and deleted, false otherwise
     */
    virtual bool deleteById(const std::string& id) = 0;
    
    /**
     * @brief Update an existing folder
     * 
     * @param folder The folder with updated information
     * @throws std::runtime_error if the folder cannot be updated
     */
    virtual void update(const Folder& folder) = 0;
    
    /**
     * @brief Check if a folder exists with the given ID
     * 
     * @param id The ID to check
     * @return True if a folder exists with that ID, false otherwise
     */
    virtual bool exists(const std::string& id) = 0;
};

#endif // FOLDERREPOSITORY_H