#ifndef INMEMORYPROJECTREPOSITORY_H
#define INMEMORYPROJECTREPOSITORY_H

#include "../../include/repositories/ProjectRepository.h"
#include <unordered_map>

/**
 * @brief TEST MOCK: In-memory implementation of ProjectRepository
 * 
 * This implementation stores projects in memory using an unordered_map.
 * This is ONLY for testing purposes - in a real application, repository
 * implementations would come from the infrastructure layer.
 */
class InMemoryProjectRepository : public ProjectRepository {
private:
    std::unordered_map<std::string, Project> storage;

public:
    /**
     * @brief Constructor
     */
    InMemoryProjectRepository() = default;
    
    /**
     * @brief Destructor
     */
    ~InMemoryProjectRepository() override = default;

    // ProjectRepository interface implementation
    std::string save(const Project& project) override;
    std::optional<Project> findById(const std::string& id) override;
    std::vector<Project> findAll() override;
    bool deleteById(const std::string& id) override;
    void update(const Project& project) override;
    bool exists(const std::string& id) override;
    
    /**
     * @brief Clear all projects from storage (useful for testing)
     */
    void clear();
    
    /**
     * @brief Get the number of projects in storage
     * 
     * @return The count of projects
     */
    size_t size() const;
};

#endif // INMEMORYPROJECTREPOSITORY_H