#ifndef PLOTTER_FILESYSTEM_DATASOURCE_H
#define PLOTTER_FILESYSTEM_DATASOURCE_H

#include "plotter_repositories/ProjectDataSource.h"
#include "plotter_repositories/FolderDataSource.h"
#include "plotter_repositories/NoteDataSource.h"
#include "plotter_filesystem_dtos/FilesystemDTOs.h"
#include <string>
#include <vector>
#include <memory>

namespace plotter {
namespace filesystem {

/**
 * @brief Filesystem-based data source for Projects
 *
 * Stores projects as directories with a .plotter_project metadata file.
 * The directory structure mirrors the logical structure of projects.
 */
class FilesystemProjectDataSource : public repositories::ProjectDataSource {
private:
    std::string name_;
    std::string rootPath_;  // Root directory where projects are stored
    bool connected_;

    std::string getProjectPath(const std::string& projectId) const;
    std::string getProjectMetadataPath(const std::string& projectId) const;
    void ensureRootDirectoryExists();

public:
    FilesystemProjectDataSource(const std::string& name, const std::string& rootPath);
    virtual ~FilesystemProjectDataSource();

    // Connection management
    void connect() override;
    void disconnect() override;
    bool isConnected() const override;
    std::string getName() const override;

    // CRUD operations
    std::string create(dto::ProjectDTO* dto) override;
    dto::ProjectDTO* read(const std::string& id) override;
    bool update(const std::string& id, dto::ProjectDTO* dto) override;
    bool remove(const std::string& id) override;
    std::vector<dto::ProjectDTO*> list() override;
};

/**
 * @brief Filesystem-based data source for Folders
 *
 * Stores folders as subdirectories with a .plotter_folder metadata file.
 */
class FilesystemFolderDataSource : public repositories::FolderDataSource {
private:
    std::string name_;
    std::string rootPath_;  // Root directory where projects are stored
    bool connected_;

    std::string getFolderPath(const std::string& folderId) const;
    std::string getFolderMetadataPath(const std::string& folderId) const;
    std::string resolveParentPath(const std::string& parentProjectId,
                                   const std::string& parentFolderId) const;
    void ensureRootDirectoryExists();
    std::vector<dto::FolderDTO*> scanFoldersInDirectory(const std::string& dirPath) const;

public:
    FilesystemFolderDataSource(const std::string& name, const std::string& rootPath);
    virtual ~FilesystemFolderDataSource();

    // Connection management
    void connect() override;
    void disconnect() override;
    bool isConnected() const override;
    std::string getName() const override;

    // CRUD operations
    std::string create(dto::FolderDTO* dto) override;
    dto::FolderDTO* read(const std::string& id) override;
    bool update(const std::string& id, dto::FolderDTO* dto) override;
    bool remove(const std::string& id) override;
    std::vector<dto::FolderDTO*> listByProject(const std::string& projectId) override;
    std::vector<dto::FolderDTO*> listByParentFolder(const std::string& folderId) override;
};

/**
 * @brief Filesystem-based data source for Notes
 *
 * Stores notes as regular files (e.g., .md, .txt) with companion
 * .plotter_meta files containing metadata.
 */
class FilesystemNoteDataSource : public repositories::NoteDataSource {
private:
    std::string name_;
    std::string rootPath_;  // Root directory where projects are stored
    bool connected_;
    std::string defaultExtension_;  // Default file extension for notes (e.g., ".md")

    std::string getNotePath(const std::string& noteId) const;
    std::string getNoteMetadataPath(const std::string& notePath) const;
    std::string resolveFolderPath(const std::string& folderId) const;
    void ensureRootDirectoryExists();
    std::vector<dto::NoteDTO*> scanNotesInDirectory(const std::string& dirPath) const;

public:
    FilesystemNoteDataSource(const std::string& name, const std::string& rootPath,
                            const std::string& defaultExtension = ".md");
    virtual ~FilesystemNoteDataSource();

    // Connection management
    void connect() override;
    void disconnect() override;
    bool isConnected() const override;
    std::string getName() const override;

    // CRUD operations
    std::string create(dto::NoteDTO* dto) override;
    dto::NoteDTO* read(const std::string& id) override;
    bool update(const std::string& id, dto::NoteDTO* dto) override;
    bool remove(const std::string& id) override;
    std::vector<dto::NoteDTO*> listByFolder(const std::string& folderId) override;

    // Note-specific operations
    std::string getContent(const std::string& id) override;
    bool updateContent(const std::string& id, const std::string& content) override;
};

} // namespace filesystem
} // namespace plotter

#endif // PLOTTER_FILESYSTEM_DATASOURCE_H

