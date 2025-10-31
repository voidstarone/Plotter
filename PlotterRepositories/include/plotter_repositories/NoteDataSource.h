#ifndef NOTE_DATASOURCE_H
#define NOTE_DATASOURCE_H

#include "DataSource.h"
#include "BaseDTOs.h"
#include <optional>
#include <vector>
#include <memory>

namespace plotter {
namespace repositories {

/**
 * @brief Abstract interface for Note data sources
 * 
 * This interface defines the contract that any concrete datasource
 * (SQLite, Redis, FileSystem, etc.) must implement to provide
 * Note data persistence using DTOs.
 * 
 * Data sources work with DTOs (inheriting from plotter::dto::NoteDTO).
 * The repository layer is responsible for converting between entities and DTOs.
 * 
 * Note: This interface handles Note metadata only. Actual note content
 * storage may be delegated to a separate content storage mechanism.
 */
class NoteDataSource : public DataSource {
public:
    virtual ~NoteDataSource() = default;
    
    /**
     * @brief Save a note DTO to the datasource
     * 
     * @param noteDTO The note DTO to save
     * @return The ID of the saved note
     * @throws std::runtime_error if the note cannot be saved
     */
    virtual std::string save(const dto::NoteDTO& noteDTO) = 0;
    
    /**
     * @brief Find a note by its ID
     * 
     * @param id The ID of the note to find
     * @return An optional containing the note DTO if found, std::nullopt otherwise
     */
    virtual std::optional<dto::NoteDTO*> findById(const std::string& id) = 0;
    
    /**
     * @brief Get all notes from the datasource
     * 
     * @return A vector containing all note DTOs
     */
    virtual std::vector<dto::NoteDTO*> findAll() = 0;
    
    /**
     * @brief Find notes by parent folder ID
     * 
     * @param parentFolderId The ID of the parent folder
     * @return A vector of note DTOs belonging to the folder
     */
    virtual std::vector<dto::NoteDTO*> findByParentFolderId(const std::string& parentFolderId) = 0;
    
    /**
     * @brief Search notes by name or content
     * 
     * @param searchTerm The term to search for
     * @return A vector of note DTOs matching the search
     */
    virtual std::vector<dto::NoteDTO*> search(const std::string& searchTerm) = 0;
    
    /**
     * @brief Delete a note by its ID
     * 
     * @param id The ID of the note to delete
     * @return True if the note was found and deleted, false otherwise
     */
    virtual bool deleteById(const std::string& id) = 0;
    
    /**
     * @brief Update an existing note DTO
     * 
     * @param noteDTO The note DTO with updated information
     * @return True if the note was updated, false if not found
     */
    virtual bool update(const dto::NoteDTO& noteDTO) = 0;
    
    /**
     * @brief Check if a note exists with the given ID
     * 
     * @param id The ID to check
     * @return True if a note exists with that ID, false otherwise
     */
    virtual bool exists(const std::string& id) = 0;
    
    /**
     * @brief Clear all notes from the datasource
     * 
     * @return Number of notes cleared
     */
    virtual size_t clear() = 0;
};

} // namespace repositories
} // namespace plotter

#endif // NOTE_DATASOURCE_H