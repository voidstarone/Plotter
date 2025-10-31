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

FilesystemNoteDataSource::FilesystemNoteDataSource(const std::string& name, const std::string& rootPath,
                                                   const std::string& defaultExtension)
    : name_(name), rootPath_(rootPath), connected_(false), defaultExtension_(defaultExtension) {
}

FilesystemNoteDataSource::~FilesystemNoteDataSource() {
    if (connected_) {
        disconnect();
    }
}

void FilesystemNoteDataSource::connect() {
    ensureRootDirectoryExists();
    connected_ = true;
}

void FilesystemNoteDataSource::disconnect() {
    connected_ = false;
}

bool FilesystemNoteDataSource::isConnected() const {
    return connected_;
}

std::string FilesystemNoteDataSource::getName() const {
    return name_;
}

void FilesystemNoteDataSource::ensureRootDirectoryExists() {
    if (!fs::exists(rootPath_)) {
        fs::create_directories(rootPath_);
    }
}

std::string FilesystemNoteDataSource::getNotePath(const std::string& noteId) const {
    // Recursively search for note by ID
    for (const auto& entry : fs::recursive_directory_iterator(rootPath_)) {
        if (entry.is_regular_file()) {
            std::string metadataPath = entry.path().string() + ".plotter_meta";
            if (fs::exists(metadataPath)) {
                std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
                Json::Value root;
                Json::Reader reader;
                if (reader.parse(content, root)) {
                    if (root["id"].asString() == noteId) {
                        return entry.path().string();
                    }
                }
            }
        }
    }
    return "";
}

std::string FilesystemNoteDataSource::getNoteMetadataPath(const std::string& notePath) const {
    return notePath + ".plotter_meta";
}

std::string FilesystemNoteDataSource::resolveFolderPath(const std::string& folderId) const {
    // Search for folder by ID
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

std::string FilesystemNoteDataSource::create(dto::NoteDTO* dto) {
    auto* fsDto = dynamic_cast<FilesystemNoteDTO*>(dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemNoteDataSource::create - DTO is not a FilesystemNoteDTO");
    }

    // Generate ID if not set
    if (fsDto->id.empty()) {
        fsDto->id = FilesystemDTOUtils::generateId();
    }

    // Resolve folder path
    std::string folderPath = resolveFolderPath(fsDto->parentFolderId);
    if (folderPath.empty()) {
        throw std::runtime_error("Parent folder not found");
    }

    // Create note file
    std::string notePath = folderPath + "/" + fsDto->name + defaultExtension_;
    fsDto->path = notePath;

    std::ofstream noteFile(notePath);
    if (!noteFile.is_open()) {
        throw std::runtime_error("Failed to create note file");
    }
    noteFile << fsDto->content;
    noteFile.close();

    // Create metadata file
    Json::Value root;
    root["id"] = fsDto->id;
    root["name"] = fsDto->name;
    root["parentFolderId"] = fsDto->parentFolderId;
    root["createdAt"] = (Json::Int64)fsDto->createdAt;
    root["updatedAt"] = (Json::Int64)fsDto->updatedAt;

    Json::StyledWriter writer;
    std::string metadataPath = getNoteMetadataPath(notePath);
    FilesystemDTOUtils::writeDotfile(metadataPath, writer.write(root));

    return fsDto->id;
}

dto::NoteDTO* FilesystemNoteDataSource::read(const std::string& id) {
    std::string notePath = getNotePath(id);
    if (notePath.empty()) {
        return nullptr;
    }

    std::string metadataPath = getNoteMetadataPath(notePath);
    std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(content, root)) {
        throw std::runtime_error("Failed to parse note metadata");
    }

    auto* dto = new FilesystemNoteDTO();
    dto->id = root["id"].asString();
    dto->name = root["name"].asString();
    dto->parentFolderId = root["parentFolderId"].asString();
    dto->createdAt = root["createdAt"].asInt64();
    dto->updatedAt = root["updatedAt"].asInt64();
    dto->path = notePath;

    // Read note content
    std::ifstream noteFile(notePath);
    if (noteFile.is_open()) {
        std::stringstream buffer;
        buffer << noteFile.rdbuf();
        dto->content = buffer.str();
    }

    return dto;
}

bool FilesystemNoteDataSource::update(const std::string& id, dto::NoteDTO* dto) {
    auto* fsDto = dynamic_cast<FilesystemNoteDTO*>(dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemNoteDataSource::update - DTO is not a FilesystemNoteDTO");
    }

    std::string notePath = getNotePath(id);
    if (notePath.empty()) {
        return false;
    }

    fsDto->updatedAt = FilesystemDTOUtils::getCurrentTimestamp();

    // Update metadata
    Json::Value root;
    root["id"] = fsDto->id;
    root["name"] = fsDto->name;
    root["parentFolderId"] = fsDto->parentFolderId;
    root["createdAt"] = (Json::Int64)fsDto->createdAt;
    root["updatedAt"] = (Json::Int64)fsDto->updatedAt;

    Json::StyledWriter writer;
    std::string metadataPath = getNoteMetadataPath(notePath);
    FilesystemDTOUtils::writeDotfile(metadataPath, writer.write(root));

    return true;
}

bool FilesystemNoteDataSource::remove(const std::string& id) {
    std::string notePath = getNotePath(id);
    if (notePath.empty()) {
        return false;
    }

    std::string metadataPath = getNoteMetadataPath(notePath);
    fs::remove(notePath);
    fs::remove(metadataPath);

    return true;
}

std::vector<dto::NoteDTO*> FilesystemNoteDataSource::scanNotesInDirectory(const std::string& dirPath) const {
    std::vector<dto::NoteDTO*> notes;

    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::string metadataPath = entry.path().string() + ".plotter_meta";
            if (fs::exists(metadataPath)) {
                try {
                    std::string content = FilesystemDTOUtils::readDotfile(metadataPath);
                    Json::Value root;
                    Json::Reader reader;
                    if (reader.parse(content, root)) {
                        auto* dto = new FilesystemNoteDTO();
                        dto->id = root["id"].asString();
                        dto->name = root["name"].asString();
                        dto->parentFolderId = root["parentFolderId"].asString();
                        dto->createdAt = root["createdAt"].asInt64();
                        dto->updatedAt = root["updatedAt"].asInt64();
                        dto->path = entry.path().string();

                        // Read note content
                        std::ifstream noteFile(dto->path);
                        if (noteFile.is_open()) {
                            std::stringstream buffer;
                            buffer << noteFile.rdbuf();
                            dto->content = buffer.str();
                        }

                        notes.push_back(dto);
                    }
                } catch (const std::exception&) {
                    continue;
                }
            }
        }
    }

    return notes;
}

std::vector<dto::NoteDTO*> FilesystemNoteDataSource::listByFolder(const std::string& folderId) {
    std::string folderPath = resolveFolderPath(folderId);
    if (folderPath.empty()) {
        return {};
    }

    return scanNotesInDirectory(folderPath);
}

std::string FilesystemNoteDataSource::getContent(const std::string& id) {
    std::string notePath = getNotePath(id);
    if (notePath.empty()) {
        throw std::runtime_error("Note not found");
    }

    std::ifstream noteFile(notePath);
    if (!noteFile.is_open()) {
        throw std::runtime_error("Failed to open note file");
    }

    std::stringstream buffer;
    buffer << noteFile.rdbuf();
    return buffer.str();
}

bool FilesystemNoteDataSource::updateContent(const std::string& id, const std::string& content) {
    std::string notePath = getNotePath(id);
    if (notePath.empty()) {
        return false;
    }

    std::ofstream noteFile(notePath);
    if (!noteFile.is_open()) {
        return false;
    }

    noteFile << content;
    noteFile.close();

    // Update timestamp in metadata
    std::string metadataPath = getNoteMetadataPath(notePath);
    std::string metaContent = FilesystemDTOUtils::readDotfile(metadataPath);
    Json::Value root;
    Json::Reader reader;
    if (reader.parse(metaContent, root)) {
        root["updatedAt"] = (Json::Int64)FilesystemDTOUtils::getCurrentTimestamp();
        Json::StyledWriter writer;
        FilesystemDTOUtils::writeDotfile(metadataPath, writer.write(root));
    }

    return true;
}

} // namespace filesystem
} // namespace plotter

