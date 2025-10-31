#ifndef PLOTTER_SQLITE_DTOS_H
#define PLOTTER_SQLITE_DTOS_H

#include "BaseDTOs.h"
#include <string>
#include <vector>

namespace plotter {
namespace sqlite_dtos {

/**
 * @brief SQLite-specific Data Transfer Object for Project database representation
 * 
 * Inherits from base ProjectDTO and defines SQLite-specific fields.
 * This DTO represents how a Project is stored in the SQLite database.
 * Folder relationships are populated by joining with the folders table.
 */
struct SqliteProjectDTO : public plotter::dto::ProjectDTO {
    std::string id;
    std::string name;
    std::string description;
    long long createdAt;        // Unix timestamp in milliseconds
    long long updatedAt;        // Unix timestamp in milliseconds
    std::vector<std::string> folderIds;  // IDs of folders in this project
    
    SqliteProjectDTO() : createdAt(0), updatedAt(0) {}
};

/**
 * @brief SQLite-specific Data Transfer Object for Folder database representation
 * 
 * Inherits from base FolderDTO and defines SQLite-specific fields.
 * This DTO represents how a Folder is stored in the SQLite database.
 * Child relationships are populated by joining with notes and folders tables.
 */
struct SqliteFolderDTO : public plotter::dto::FolderDTO {
    std::string id;
    std::string name;
    std::string description;
    std::string parentProjectId;
    std::string parentFolderId;
    long long createdAt;
    long long updatedAt;
    std::vector<std::string> noteIds;       // IDs of notes in this folder
    std::vector<std::string> subfolderIds;  // IDs of subfolders
    
    SqliteFolderDTO() : createdAt(0), updatedAt(0) {}
};

/**
 * @brief SQLite-specific Data Transfer Object for Note database representation
 * 
 * Inherits from base NoteDTO and defines SQLite-specific fields.
 * This DTO represents how Note metadata is stored in the SQLite database.
 */
struct SqliteNoteDTO : public plotter::dto::NoteDTO {
    std::string id;
    std::string name;
    std::string path;
    std::string content;
    std::string parentFolderId;
    long long createdAt;
    long long updatedAt;
    
    SqliteNoteDTO() : createdAt(0), updatedAt(0) {}
};

/**
 * @brief Utility functions for SQLite-specific DTO operations
 */
class SqliteDTOUtils {
public:
    /**
     * @brief Get current timestamp in milliseconds
     * @return Unix timestamp in milliseconds
     */
    static long long getCurrentTimestamp();
};

} // namespace sqlite_dtos
} // namespace plotter

#endif // PLOTTER_SQLITE_DTOS_H
