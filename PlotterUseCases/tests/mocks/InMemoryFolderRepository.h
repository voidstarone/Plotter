#ifndef INMEMORYFOLDERREPOSITORY_H
#define INMEMORYFOLDERREPOSITORY_H

#include "../../include/repositories/FolderRepository.h"
#include <unordered_map>

/**
 * @brief TEST MOCK: In-memory implementation of FolderRepository
 * 
 * This implementation stores folders in memory using an unordered_map.
 * This is ONLY for testing purposes - in a real application, repository
 * implementations would come from the infrastructure layer.
 */
class InMemoryFolderRepository : public FolderRepository {
private:
    std::unordered_map<std::string, Folder> storage;

public:
    /**
     * @brief Constructor
     */
    InMemoryFolderRepository() = default;
    
    /**
     * @brief Destructor
     */
    ~InMemoryFolderRepository() override = default;

    // FolderRepository interface implementation
    std::string save(const Folder& folder) override;
    std::optional<Folder> findById(const std::string& id) override;
    std::vector<Folder> findAll() override;
    std::vector<Folder> findByParentProjectId(const std::string& parentProjectId) override;
    std::vector<Folder> findByParentFolderId(const std::string& parentFolderId) override;
    bool deleteById(const std::string& id) override;
    void update(const Folder& folder) override;
    bool exists(const std::string& id) override;
    
    /**
     * @brief Clear all folders from storage (useful for testing)
     */
    void clear();
    
    /**
     * @brief Get the number of folders in storage
     * 
     * @return The count of folders
     */
    size_t size() const;
};

#endif // INMEMORYFOLDERREPOSITORY_H