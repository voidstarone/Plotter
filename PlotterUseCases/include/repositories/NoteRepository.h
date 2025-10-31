#ifndef NOTEREPOSITORY_H
#define NOTEREPOSITORY_H

#include "Note.h"
#include <memory>
#include <optional>
#include <vector>

/**
 * @brief Abstract repository interface for Note entities
 * 
 * This interface defines the contract for persisting and retrieving Note entities.
 * Note that this repository manages the Note metadata (id, name, path, parent folder, etc.)
 * but the actual content is handled by the NoteStorage interface.
 */
class NoteRepository {
public:
    virtual ~NoteRepository() = default;
    
    /**
     * @brief Save a note to the repository
     * 
     * @param note The note to save
     * @return The ID of the saved note
     * @throws std::runtime_error if the note cannot be saved
     */
    virtual std::string save(const Note& note) = 0;
    
    /**
     * @brief Find a note by its ID
     * 
     * @param id The ID of the note to find
     * @return An optional containing the note if found, std::nullopt otherwise
     */
    virtual std::optional<Note> findById(const std::string& id) = 0;
    
    /**
     * @brief Get all notes in the repository
     * 
     * @return A vector containing all notes
     */
    virtual std::vector<Note> findAll() = 0;
    
    /**
     * @brief Find notes by parent folder ID
     * 
     * @param parentFolderId The ID of the parent folder
     * @return A vector containing notes that belong to the specified folder
     */
    virtual std::vector<Note> findByParentFolderId(const std::string& parentFolderId) = 0;
    
    /**
     * @brief Search notes by name or content
     * 
     * @param searchTerm The term to search for in note names or content
     * @return A vector containing notes that match the search criteria
     */
    virtual std::vector<Note> search(const std::string& searchTerm) = 0;
    
    /**
     * @brief Delete a note by its ID
     * 
     * @param id The ID of the note to delete
     * @return True if the note was found and deleted, false otherwise
     */
    virtual bool deleteById(const std::string& id) = 0;
    
    /**
     * @brief Update an existing note
     * 
     * @param note The note with updated information
     * @throws std::runtime_error if the note cannot be updated
     */
    virtual void update(const Note& note) = 0;
    
    /**
     * @brief Check if a note exists with the given ID
     * 
     * @param id The ID to check
     * @return True if a note exists with that ID, false otherwise
     */
    virtual bool exists(const std::string& id) = 0;
};

#endif // NOTEREPOSITORY_H