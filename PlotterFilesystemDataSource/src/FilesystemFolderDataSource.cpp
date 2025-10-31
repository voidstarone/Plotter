#include "plotter_filesystem/FilesystemDataSource.h"
#include "plotter_filesystem_dtos/FilesystemDTOs.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <json/json.h>

namespace fs = std::filesystem;
using namespace plotter::filesystem_dtos;

namespace plotter {
namespace filesystem {

FilesystemFolderDataSource::FilesystemFolderDataSource(const std::string& name, const std::string& rootPath)
    : name_(name), rootPath_(rootPath), connected_(false) {
}

FilesystemFolderDataSource::~FilesystemFolderDataSource() {
    if (connected_) {
        disconnect();
    }
}

void FilesystemFolderDataSource::connect() {
    ensureRootDirectoryExists();
    connected_ = true;
}

void FilesystemFolderDataSource::disconnect() {
    connected_ = false;
}

bool FilesystemFolderDataSource::isConnected() const {
    return connected_;
}

std::string FilesystemFolderDataSource::getName() const {
    return name_;
}

void FilesystemFolderDataSource::ensureRootDirectoryExists() {
    if (!fs::exists(rootPath_)) {
        fs::create_directories(rootPath_);
    }
}

std::string FilesystemFolderDataSource::getFolderPath(const std::string& folderId) const {
    // Recursively search for folder by ID
    for (const auto& entry : fs::recursive_directory_iterator(rootPath_)) {
        if (entry.is_directory()) {
            std::string metadataPath = entry.path().string() + "/.plotter_folder";
            if (fs::exists(metadataPath)) {
                std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
                Json::Value root;
                Json::Reader reader;
                if (reader.parse(content, root)) {
                    if (root["id"].asString() == folderId) {
                        return entry.path().string();
                    }
                }
            }
        }
    }
    return "";
}

std::string FilesystemFolderDataSource::getFolderMetadataPath(const std::string& folderId) const {
    std::string folderPath = getFolderPath(folderId);
    if (folderPath.empty()) {
        return "";
    }
    return folderPath + "/.plotter_folder";
}

std::string FilesystemFolderDataSource::resolveParentPath(const std::string& parentProjectId,
                                                           const std::string& parentFolderId) const {
    if (!parentFolderId.empty()) {
        return getFolderPath(parentFolderId);
    }

    // Find project directory
    for (const auto& entry : fs::directory_iterator(rootPath_)) {
        if (entry.is_directory()) {
            std::string metadataPath = entry.path().string() + "/.plotter_project";
            if (fs::exists(metadataPath)) {
                std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
                Json::Value root;
                Json::Reader reader;
                if (reader.parse(content, root)) {
                    if (root["id"].asString() == parentProjectId) {
                        return entry.path().string();
                    }
                }
            }
        }
    }
    return "";
}

std::string FilesystemFolderDataSource::create(dto::FolderDTO* dto) {
    auto* fsDto = dynamic_cast<FilesystemFolderDTO*>(dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemFolderDataSource::create - DTO is not a FilesystemFolderDTO");
    }

    // Generate ID if not set
    if (fsDto->id.empty()) {
        fsDto->id = FilesystemDTOUtils::generateId();
    }

    // Resolve parent path
    std::string parentPath = resolveParentPath(fsDto->parentProjectId, fsDto->parentFolderId);
    if (parentPath.empty()) {
        throw std::runtime_error("Parent directory not found");
    }

    // Create folder directory
    std::string folderPath = parentPath + "/" + fsDto->name;
    fs::create_directories(folderPath);
    fsDto->path = folderPath;

    // Create metadata file
    Json::Value root;
    root["id"] = fsDto->id;
    root["name"] = fsDto->name;
    root["description"] = fsDto->description;
    root["parentProjectId"] = fsDto->parentProjectId;
    root["parentFolderId"] = fsDto->parentFolderId;
    root["createdAt"] = (Json::Int64)fsDto->createdAt;
    root["updatedAt"] = (Json::Int64)fsDto->updatedAt;

    Json::Value noteIds(Json::arrayValue);
    for (const auto& id : fsDto->noteIds) {
        noteIds.append(id);
    }
    root["noteIds"] = noteIds;

    Json::Value subfolderIds(Json::arrayValue);
    for (const auto& id : fsDto->subfolderIds) {
        subfolderIds.append(id);
    }
    root["subfolderIds"] = subfolderIds;

    Json::StyledWriter writer;
    std::string metadataPath = folderPath + "/.plotter_folder";
    FilesystemDTOUtils::writeDotfile(metadataPath, writer.write(root));

    return fsDto->id;
}

dto::FolderDTO* FilesystemFolderDataSource::read(const std::string& id) {
    std::string metadataPath = getFolderMetadataPath(id);
    if (metadataPath.empty()) {
        return nullptr;
    }

    std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(content, root)) {
        throw std::runtime_error("Failed to parse folder metadata");
    }

    auto* dto = new FilesystemFolderDTO();
    dto->id = root["id"].asString();
    dto->name = root["name"].asString();
    dto->description = root["description"].asString();
    dto->parentProjectId = root["parentProjectId"].asString();
    dto->parentFolderId = root["parentFolderId"].asString();
    dto->createdAt = root["createdAt"].asInt64();
    dto->updatedAt = root["updatedAt"].asInt64();
    dto->path = getFolderPath(id);

    const Json::Value noteIds = root["noteIds"];
    for (const auto& noteId : noteIds) {
        dto->noteIds.push_back(noteId.asString());
    }

    const Json::Value subfolderIds = root["subfolderIds"];
    for (const auto& subfolderId : subfolderIds) {
        dto->subfolderIds.push_back(subfolderId.asString());
    }

    return dto;
}

bool FilesystemFolderDataSource::update(const std::string& id, dto::FolderDTO* dto) {
    auto* fsDto = dynamic_cast<FilesystemFolderDTO*>(dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemFolderDataSource::update - DTO is not a FilesystemFolderDTO");
    }

    std::string metadataPath = getFolderMetadataPath(id);
    if (metadataPath.empty()) {
        return false;
    }

    fsDto->updatedAt = FilesystemDTOUtils::getCurrentTimestamp();

    Json::Value root;
    root["id"] = fsDto->id;
    root["name"] = fsDto->name;
    root["description"] = fsDto->description;
    root["parentProjectId"] = fsDto->parentProjectId;
    root["parentFolderId"] = fsDto->parentFolderId;
    root["createdAt"] = (Json::Int64)fsDto->createdAt;
    root["updatedAt"] = (Json::Int64)fsDto->updatedAt;

    Json::Value noteIds(Json::arrayValue);
    for (const auto& nid : fsDto->noteIds) {
        noteIds.append(nid);
    }
    root["noteIds"] = noteIds;

    Json::Value subfolderIds(Json::arrayValue);
    for (const auto& sid : fsDto->subfolderIds) {
        subfolderIds.append(sid);
    }
    root["subfolderIds"] = subfolderIds;

    Json::StyledWriter writer;
    FilesystemDTOUtils::writeDotfile(metadataPath, writer.write(root));

    return true;
}

bool FilesystemFolderDataSource::remove(const std::string& id) {
    std::string folderPath = getFolderPath(id);
    if (folderPath.empty()) {
        return false;
    }

    fs::remove_all(folderPath);
    return true;
}

std::vector<dto::FolderDTO*> FilesystemFolderDataSource::scanFoldersInDirectory(const std::string& dirPath) const {
    std::vector<dto::FolderDTO*> folders;

    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_directory()) {
            std::string metadataPath = entry.path().string() + "/.plotter_folder";
            if (fs::exists(metadataPath)) {
                try {
                    std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
                    Json::Value root;
                    Json::Reader reader;
                    if (reader.parse(content, root)) {
                        auto* dto = new FilesystemFolderDTO();
                        dto->id = root["id"].asString();
                        dto->name = root["name"].asString();
                        dto->description = root["description"].asString();
                        dto->parentProjectId = root["parentProjectId"].asString();
                        dto->parentFolderId = root["parentFolderId"].asString();
                        dto->createdAt = root["createdAt"].asInt64();
                        dto->updatedAt = root["updatedAt"].asInt64();
                        dto->path = entry.path().string();

                        const Json::Value noteIds = root["noteIds"];
                        for (const auto& noteId : noteIds) {
                            dto->noteIds.push_back(noteId.asString());
                        }

                        const Json::Value subfolderIds = root["subfolderIds"];
                        for (const auto& subfolderId : subfolderIds) {
                            dto->subfolderIds.push_back(subfolderId.asString());
                        }

                        folders.push_back(dto);
                    }
                } catch (const std::exception&) {
                    continue;
                }
            }
        }
    }

    return folders;
}

std::vector<dto::FolderDTO*> FilesystemFolderDataSource::listByProject(const std::string& projectId) {
    std::string projectPath = resolveParentPath(projectId, "");
    if (projectPath.empty()) {
        return {};
    }

    return scanFoldersInDirectory(projectPath);
}

std::vector<dto::FolderDTO*> FilesystemFolderDataSource::listByParentFolder(const std::string& folderId) {
    std::string folderPath = getFolderPath(folderId);
    if (folderPath.empty()) {
        return {};
    }

    return scanFoldersInDirectory(folderPath);
}

} // namespace filesystem
} // namespace plotter

