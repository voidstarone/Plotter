#ifndef PLOTTER_FILESYSTEM_DTOS_H
#define PLOTTER_FILESYSTEM_DTOS_H

#include "BaseDTOs.h"
#include <string>
#include <vector>

namespace plotter {
namespace filesystem_dtos {

/**
 * @brief Filesystem-specific Data Transfer Object for Project representation
 *
 * Inherits from base ProjectDTO and defines filesystem-specific fields.
 * Projects are represented as top-level directories with a hidden .plotter_project file
 * containing the project metadata (ID, description, timestamps).
 */
struct FilesystemProjectDTO : public plotter::dto::ProjectDTO {
    std::string id;
    std::string name;               // Also the directory name
    std::string description;
    std::string rootPath;           // Absolute path to the project directory
    long long createdAt;            // Unix timestamp in milliseconds
    long long updatedAt;            // Unix timestamp in milliseconds
    std::vector<std::string> folderIds;  // IDs of folders in this project

    FilesystemProjectDTO() : createdAt(0), updatedAt(0) {}
};

/**
 * @brief Filesystem-specific Data Transfer Object for Folder representation
 *
 * Inherits from base FolderDTO and defines filesystem-specific fields.
 * Folders are represented as directories with a hidden .plotter_folder file
 * containing the folder metadata (ID, description, parent relationships, timestamps).
 */
struct FilesystemFolderDTO : public plotter::dto::FolderDTO {
    std::string id;
    std::string name;               // Also the directory name
    std::string description;
    std::string path;               // Absolute path to the folder directory
    std::string parentProjectId;
    std::string parentFolderId;
    long long createdAt;
    long long updatedAt;
    std::vector<std::string> noteIds;       // IDs of notes in this folder
    std::vector<std::string> subfolderIds;  // IDs of subfolders

    FilesystemFolderDTO() : createdAt(0), updatedAt(0) {}
};

/**
 * @brief Filesystem-specific Data Transfer Object for Note representation
 *
 * Inherits from base NoteDTO and defines filesystem-specific fields.
 * Notes are represented as regular text files with a companion hidden file
 * (e.g., note.md and .plotter_note.md.meta) containing note metadata.
 */
struct FilesystemNoteDTO : public plotter::dto::NoteDTO {
    std::string id;
    std::string name;               // Filename without extension
    std::string path;               // Absolute path to the note file
    std::string content;            // Content of the note file
    std::string parentFolderId;
    long long createdAt;
    long long updatedAt;

    FilesystemNoteDTO() : createdAt(0), updatedAt(0) {}
};

/**
 * @brief Utility functions for Filesystem-specific DTO operations
 */
class FilesystemDTOUtils {
public:
    /**
     * @brief Get current timestamp in milliseconds
     * @return Unix timestamp in milliseconds
     */
    static long long getCurrentTimestamp();

    /**
     * @brief Generate a unique ID for filesystem entities
     * @return A unique string ID (UUID format)
     */
    static std::string generateId();

    /**
     * @brief Read metadata from a dotfile
     * @param dotfilePath Path to the dotfile
     * @return JSON string containing metadata
     */
    static std::string readDotfile(const std::string& dotfilePath);

    /**
     * @brief Write metadata to a dotfile
     * @param dotfilePath Path to the dotfile
     * @param content JSON string containing metadata
     */
    static void writeDotfile(const std::string& dotfilePath, const std::string& content);

    /**
     * @brief Check if a path is a Plotter project directory
     * @param path Path to check
     * @return True if the directory contains .plotter_project file
     */
    static bool isProjectDirectory(const std::string& path);

    /**
     * @brief Check if a path is a Plotter folder directory
     * @param path Path to check
     * @return True if the directory contains .plotter_folder file
     */
    static bool isFolderDirectory(const std::string& path);

    /**
     * @brief Check if a path is a Plotter note file
     * @param path Path to check
     * @return True if there's a corresponding .plotter_note.*.meta file
     */
    static bool isNoteFile(const std::string& path);
};

} // namespace filesystem_dtos
} // namespace plotter

#endif // PLOTTER_FILESYSTEM_DTOS_H

