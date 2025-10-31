#ifndef PROJECT_H
#define PROJECT_H

#include "FileItem.h"
#include <string>
#include <vector>

/**
 * @brief Represents a project entity that can contain folders
 * 
 * Project is a simple entity that stores IDs of top-level folders.
 * It extends FileItem to inherit id, name, and type properties.
 * Relationships are managed through ID references rather than embedded objects.
 */
class Project : public FileItem {
private:
    std::string description;
    std::vector<std::string> folderIds; // IDs of top-level folders in this project

public:
    /**
     * @brief Constructor for Project entity
     * 
     * @param id Unique identifier for this project
     * @param name The name of the project
     * @param description Description of the project's purpose
     */
    Project(const std::string& id, const std::string& name, const std::string& description);

    /**
     * @brief Get the description
     * 
     * @return Const reference to the project's description
     */
    const std::string& getDescription() const;

    /**
     * @brief Set the description
     * 
     * @param description The new description
     */
    void setDescription(const std::string& description);

    /**
     * @brief Get all folder IDs
     * 
     * @return Const reference to the vector of folder IDs
     */
    const std::vector<std::string>& getFolderIds() const;

    /**
     * @brief Add a folder ID to this project
     * 
     * @param folderId The ID of the folder to add
     */
    void addFolderId(const std::string& folderId);

    /**
     * @brief Remove a folder ID from this project
     * 
     * @param folderId The ID of the folder to remove
     * @return True if the folder ID was found and removed, false otherwise
     */
    bool removeFolderId(const std::string& folderId);
};

#endif // PROJECT_H
