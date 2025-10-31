#ifndef NOTESTORAGE_H
#define NOTESTORAGE_H

#include <string>

/**
 * @brief Abstract interface for loading and saving note content.
 * 
 * This interface allows different storage backends (filesystem, database, cloud, etc.)
 * to be used with the Note class. Notes are loaded lazily based on their path,
 * avoiding the need to keep all note content in memory.
 */
class NoteStorage {
public:
    virtual ~NoteStorage() = default;

    /**
     * @brief Load note content from storage based on a path.
     * 
     * @param path The unique path identifier for the note (e.g., file path, database key)
     * @return The content of the note as a string
     * @throws std::runtime_error if the note cannot be loaded
     */
    virtual std::string loadNote(const std::string& path) = 0;

    /**
     * @brief Save note content to storage at the specified path.
     * 
     * @param path The unique path identifier for the note
     * @param content The content to save
     * @throws std::runtime_error if the note cannot be saved
     */
    virtual void saveNote(const std::string& path, const std::string& content) = 0;

    /**
     * @brief Check if a note exists at the specified path.
     * 
     * @param path The unique path identifier for the note
     * @return true if the note exists, false otherwise
     */
    virtual bool noteExists(const std::string& path) = 0;
};

#endif // NOTESTORAGE_H
