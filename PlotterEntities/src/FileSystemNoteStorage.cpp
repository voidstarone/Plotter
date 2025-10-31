#include "FileSystemNoteStorage.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

FileSystemNoteStorage::FileSystemNoteStorage(const std::string& baseDir) 
    : baseDirectory(baseDir) {
    // Create base directory if it doesn't exist
    std::filesystem::create_directories(baseDirectory);
}

std::string FileSystemNoteStorage::loadNote(const std::string& path) {
    std::string fullPath = baseDirectory + "/" + path;
    std::ifstream file(fullPath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to load note from: " + fullPath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileSystemNoteStorage::saveNote(const std::string& path, const std::string& content) {
    std::string fullPath = baseDirectory + "/" + path;
    
    // Create parent directories if needed
    std::filesystem::path filePath(fullPath);
    std::filesystem::create_directories(filePath.parent_path());

    std::ofstream file(fullPath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to save note to: " + fullPath);
    }

    file << content;
}

bool FileSystemNoteStorage::noteExists(const std::string& path) {
    std::string fullPath = baseDirectory + "/" + path;
    return std::filesystem::exists(fullPath);
}
