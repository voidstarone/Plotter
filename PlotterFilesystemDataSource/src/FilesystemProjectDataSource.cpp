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

FilesystemProjectDataSource::FilesystemProjectDataSource(const std::string& name, const std::string& rootPath)
    : name_(name), rootPath_(rootPath), connected_(false) {
}

FilesystemProjectDataSource::~FilesystemProjectDataSource() {
    if (connected_) {
        disconnect();
    }
}

void FilesystemProjectDataSource::connect() {
    ensureRootDirectoryExists();
    connected_ = true;
}

void FilesystemProjectDataSource::disconnect() {
    connected_ = false;
}

bool FilesystemProjectDataSource::isConnected() const {
    return connected_;
}

std::string FilesystemProjectDataSource::getName() const {
    return name_;
}

void FilesystemProjectDataSource::ensureRootDirectoryExists() {
    if (!fs::exists(rootPath_)) {
        fs::create_directories(rootPath_);
    }
}

std::string FilesystemProjectDataSource::getProjectPath(const std::string& projectId) const {
    // Find project by scanning directories
    for (const auto& entry : fs::directory_iterator(rootPath_)) {
        if (entry.is_directory()) {
            std::string metadataPath = entry.path().string() + "/.plotter_project";
            if (fs::exists(metadataPath)) {
                std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
                Json::Value root;
                Json::Reader reader;
                if (reader.parse(content, root)) {
                    if (root["id"].asString() == projectId) {
                        return entry.path().string();
                    }
                }
            }
        }
    }
    return "";
}

std::string FilesystemProjectDataSource::getProjectMetadataPath(const std::string& projectId) const {
    std::string projectPath = getProjectPath(projectId);
    if (projectPath.empty()) {
        return "";
    }
    return projectPath + "/.plotter_project";
}

std::string FilesystemProjectDataSource::create(dto::ProjectDTO* dto) {
    auto* fsDto = dynamic_cast<FilesystemProjectDTO*>(dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemProjectDataSource::create - DTO is not a FilesystemProjectDTO");
    }

    // Generate ID if not set
    if (fsDto->id.empty()) {
        fsDto->id = FilesystemDTOUtils::generateId();
    }

    // Create project directory
    std::string projectPath = rootPath_ + "/" + fsDto->name;
    fs::create_directories(projectPath);
    fsDto->rootPath = projectPath;

    // Create metadata file
    Json::Value root;
    root["id"] = fsDto->id;
    root["name"] = fsDto->name;
    root["description"] = fsDto->description;
    root["createdAt"] = (Json::Int64)fsDto->createdAt;
    root["updatedAt"] = (Json::Int64)fsDto->updatedAt;

    Json::Value folderIds(Json::arrayValue);
    for (const auto& id : fsDto->folderIds) {
        folderIds.append(id);
    }
    root["folderIds"] = folderIds;

    Json::StyledWriter writer;
    std::string metadataPath = projectPath + "/.plotter_project";
    FilesystemDTOUtils::writeDotfile(metadataPath, writer.write(root));

    return fsDto->id;
}

dto::ProjectDTO* FilesystemProjectDataSource::read(const std::string& id) {
    std::string metadataPath = getProjectMetadataPath(id);
    if (metadataPath.empty()) {
        return nullptr;
    }

    std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(content, root)) {
        throw std::runtime_error("Failed to parse project metadata");
    }

    auto* dto = new FilesystemProjectDTO();
    dto->id = root["id"].asString();
    dto->name = root["name"].asString();
    dto->description = root["description"].asString();
    dto->createdAt = root["createdAt"].asInt64();
    dto->updatedAt = root["updatedAt"].asInt64();
    dto->rootPath = getProjectPath(id);

    const Json::Value folderIds = root["folderIds"];
    for (const auto& folderId : folderIds) {
        dto->folderIds.push_back(folderId.asString());
    }

    return dto;
}

bool FilesystemProjectDataSource::update(const std::string& id, dto::ProjectDTO* dto) {
    auto* fsDto = dynamic_cast<FilesystemProjectDTO*>(dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemProjectDataSource::update - DTO is not a FilesystemProjectDTO");
    }

    std::string metadataPath = getProjectMetadataPath(id);
    if (metadataPath.empty()) {
        return false;
    }

    fsDto->updatedAt = FilesystemDTOUtils::getCurrentTimestamp();

    Json::Value root;
    root["id"] = fsDto->id;
    root["name"] = fsDto->name;
    root["description"] = fsDto->description;
    root["createdAt"] = (Json::Int64)fsDto->createdAt;
    root["updatedAt"] = (Json::Int64)fsDto->updatedAt;

    Json::Value folderIds(Json::arrayValue);
    for (const auto& fid : fsDto->folderIds) {
        folderIds.append(fid);
    }
    root["folderIds"] = folderIds;

    Json::StyledWriter writer;
    FilesystemDTOUtils::writeDotfile(metadataPath, writer.write(root));

    return true;
}

bool FilesystemProjectDataSource::remove(const std::string& id) {
    std::string projectPath = getProjectPath(id);
    if (projectPath.empty()) {
        return false;
    }

    fs::remove_all(projectPath);
    return true;
}

std::vector<dto::ProjectDTO*> FilesystemProjectDataSource::list() {
    std::vector<dto::ProjectDTO*> projects;

    for (const auto& entry : fs::directory_iterator(rootPath_)) {
        if (entry.is_directory()) {
            std::string metadataPath = entry.path().string() + "/.plotter_project";
            if (fs::exists(metadataPath)) {
                try {
                    std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
                    Json::Value root;
                    Json::Reader reader;
                    if (reader.parse(content, root)) {
                        auto* dto = new FilesystemProjectDTO();
                        dto->id = root["id"].asString();
                        dto->name = root["name"].asString();
                        dto->description = root["description"].asString();
                        dto->createdAt = root["createdAt"].asInt64();
                        dto->updatedAt = root["updatedAt"].asInt64();
                        dto->rootPath = entry.path().string();

                        const Json::Value folderIds = root["folderIds"];
                        for (const auto& folderId : folderIds) {
                            dto->folderIds.push_back(folderId.asString());
                        }

                        projects.push_back(dto);
                    }
                } catch (const std::exception&) {
                    // Skip invalid projects
                    continue;
                }
            }
        }
    }

    return projects;
}

} // namespace filesystem
} // namespace plotter

