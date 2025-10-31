#ifndef INMEMORYNOTEREPOSIORY_H
#define INMEMORYNOTEREPOSIORY_H

#include "../../include/repositories/NoteRepository.h"
#include "NoteStorage.h"
#include <unordered_map>

/**
 * @brief TEST MOCK: In-memory implementation of NoteRepository
 * 
 * This implementation stores notes in memory using an unordered_map.
 * This is ONLY for testing purposes - in a real application, repository
 * implementations would come from the infrastructure layer.
 * Note content is handled by the injected NoteStorage implementation.
 */
class InMemoryNoteRepository : public NoteRepository {
private:
    std::unordered_map<std::string, Note> storage;
    std::shared_ptr<NoteStorage> noteStorage;

public:
    /**
     * @brief Constructor
     * 
     * @param storage The NoteStorage implementation to use for content persistence
     */
    explicit InMemoryNoteRepository(std::shared_ptr<NoteStorage> storage);
    
    /**
     * @brief Destructor
     */
    ~InMemoryNoteRepository() override = default;

    // NoteRepository interface implementation
    std::string save(const Note& note) override;
    std::optional<Note> findById(const std::string& id) override;
    std::vector<Note> findAll() override;
    std::vector<Note> findByParentFolderId(const std::string& parentFolderId) override;
    std::vector<Note> search(const std::string& searchTerm) override;
    bool deleteById(const std::string& id) override;
    void update(const Note& note) override;
    bool exists(const std::string& id) override;
    
    /**
     * @brief Clear all notes from storage (useful for testing)
     */
    void clear();
    
    /**
     * @brief Get the number of notes in storage
     * 
     * @return The count of notes
     */
    size_t size() const;
    
    /**
     * @brief Get the NoteStorage instance being used
     * 
     * @return Shared pointer to the NoteStorage
     */
    std::shared_ptr<NoteStorage> getNoteStorage() const;
};

#endif // INMEMORYNOTEREPOSIORY_H