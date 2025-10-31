#ifndef FILESYSTEMNOTESTORAGE_H
#define FILESYSTEMNOTESTORAGE_H

#include "NoteStorage.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

/**
 * @brief Filesystem-based implementation of NoteStorage.
 * 
 * Stores notes as text files on disk, allowing notes to be loaded
 * lazily only when accessed. This prevents loading all notes into memory.
 */
class FileSystemNoteStorage : public NoteStorage {
private:
    std::string baseDirectory;

public:
    /**
     * @brief Construct a new FileSystemNoteStorage object.
     * 
     * @param baseDir The base directory where notes will be stored
     */
    explicit FileSystemNoteStorage(const std::string& baseDir);

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

#endif // FILESYSTEMNOTESTORAGE_H
