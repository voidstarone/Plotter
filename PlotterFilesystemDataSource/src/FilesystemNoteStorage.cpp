#include "plotter_filesystem/FilesystemNoteStorage.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace plotter {
namespace filesystem {

FilesystemNoteStorage::FilesystemNoteStorage(const std::string& baseDir)
    : baseDirectory(baseDir) {
    // Create base directory if it doesn't exist
    std::filesystem::create_directories(baseDirectory);
}

std::string FilesystemNoteStorage::loadNote(const std::string& path) {
    std::string fullPath = baseDirectory + "/" + path;
    std::ifstream file(fullPath);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to load note from: " + fullPath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FilesystemNoteStorage::saveNote(const std::string& path, const std::string& content) {
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

bool FilesystemNoteStorage::noteExists(const std::string& path) {
    std::string fullPath = baseDirectory + "/" + path;
    return std::filesystem::exists(fullPath);
}

} // namespace filesystem
} // namespace plotter
#ifndef PLOTTER_FILESYSTEM_NOTE_STORAGE_H
#define PLOTTER_FILESYSTEM_NOTE_STORAGE_H

#include "NoteStorage.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace plotter {
namespace filesystem {

/**
 * @brief Filesystem-based implementation of NoteStorage.
 *
 * Stores notes as text files on disk, allowing notes to be loaded
 * lazily only when accessed. This prevents loading all notes into memory.
 *
 * This is an INFRASTRUCTURE component and should NOT be in the domain layer.
 */
class FilesystemNoteStorage : public NoteStorage {
private:
    std::string baseDirectory;

public:
    /**
     * @brief Construct a new FilesystemNoteStorage object.
     *
     * @param baseDir The base directory where notes will be stored
     */
    explicit FilesystemNoteStorage(const std::string& baseDir);

    /**
     * @brief Load note content from a file.
     *
     * @param path Relative path to the note file (relative to baseDirectory)
     * @return The content of the note
     * @throws std::runtime_error if the file cannot be read
     */
    std::string loadNote(const std::string& path) override;

    /**
     * @brief Save note content to a file.
     *
     * @param path Relative path to the note file (relative to baseDirectory)
     * @param content The content to write
     * @throws std::runtime_error if the file cannot be written
     */
    void saveNote(const std::string& path, const std::string& content) override;

    /**
     * @brief Check if a note file exists.
     *
     * @param path Relative path to the note file
     * @return true if the file exists, false otherwise
     */
    bool noteExists(const std::string& path) override;
};

} // namespace filesystem
} // namespace plotter

#endif // PLOTTER_FILESYSTEM_NOTE_STORAGE_H

