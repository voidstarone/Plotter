#ifndef FOLDER_H
#define FOLDER_H

#include "FileItem.h"
#include <string>
#include <vector>

/**
 * @brief Represents a folder entity that can contain notes and other folders
 * 
 * Folder is a simple entity that stores IDs of child items (notes and subfolders)
 * and its parent. It extends FileItem to inherit id, name, and type properties.
 * Relationships are managed through ID references rather than embedded objects.
 */
class Folder : public FileItem {
private:
    std::string description;
    std::string parentProjectId; // ID of parent project (empty if top-level in project)
    std::string parentFolderId;  // ID of parent folder (empty if top-level)
    std::vector<std::string> noteIds; // IDs of notes in this folder
    std::vector<std::string> subfolderIds; // IDs of subfolders

public:
    /**
     * @brief Constructor for Folder entity
     * 
     * @param id Unique identifier for this folder
     * @param name The name of the folder
     * @param description Description of the folder's purpose or contents
     * @param parentProjectId ID of the parent project (empty string if none)
     * @param parentFolderId ID of the parent folder (empty string if top-level)
     */
    Folder(const std::string& id, const std::string& name, const std::string& description,
           const std::string& parentProjectId, const std::string& parentFolderId);

    /**
     * @brief Get the description
     * 
     * @return Const reference to the folder's description
     */
    const std::string& getDescription() const;

    /**
     * @brief Set the description
     * 
     * @param description The new description
     */
    void setDescription(const std::string& description);

    /**
     * @brief Get the parent project ID
     * 
     * @return Const reference to the parent project's ID
     */
    const std::string& getParentProjectId() const;

    /**
     * @brief Set the parent project ID
     * 
     * @param parentProjectId The ID of the parent project
     */
    void setParentProjectId(const std::string& parentProjectId);

    /**
     * @brief Get the parent folder ID
     * 
     * @return Const reference to the parent folder's ID
     */
    const std::string& getParentFolderId() const;

    /**
     * @brief Set the parent folder ID
     * 
     * @param parentFolderId The ID of the parent folder
     */
    void setParentFolderId(const std::string& parentFolderId);

    /**
     * @brief Get all note IDs
     * 
     * @return Const reference to the vector of note IDs
     */
    const std::vector<std::string>& getNoteIds() const;

    /**
     * @brief Add a note ID to this folder
     * 
     * @param noteId The ID of the note to add
     */
    void addNoteId(const std::string& noteId);

    /**
     * @brief Remove a note ID from this folder
     * 
     * @param noteId The ID of the note to remove
     * @return True if the note ID was found and removed, false otherwise
     */
    bool removeNoteId(const std::string& noteId);

    /**
     * @brief Get all subfolder IDs
     * 
     * @return Const reference to the vector of subfolder IDs
     */
    const std::vector<std::string>& getSubfolderIds() const;

    /**
     * @brief Add a subfolder ID to this folder
     * 
     * @param subfolderId The ID of the subfolder to add
     */
    void addSubfolderId(const std::string& subfolderId);

    /**
     * @brief Remove a subfolder ID from this folder
     * 
     * @param subfolderId The ID of the subfolder to remove
     * @return True if the subfolder ID was found and removed, false otherwise
     */
    bool removeSubfolderId(const std::string& subfolderId);
};

#endif // FOLDER_H
