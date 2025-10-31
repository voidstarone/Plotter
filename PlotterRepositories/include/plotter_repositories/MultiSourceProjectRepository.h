#ifndef MULTISOURCE_PROJECT_REPOSITORY_H
#define MULTISOURCE_PROJECT_REPOSITORY_H

#include "repositories/ProjectRepository.h"
#include "plotter_repositories/ProjectDataSource.h"
#include "plotter_repositories/DataSourceRouter.h"
#include "plotter_repositories/EntityDTOMapper.h"
#include <memory>
#include <stdexcept>
#include <sstream>

namespace plotter {
namespace repositories {

/**
 * @brief Multi-source implementation of ProjectRepository
 * 
 * Template parameter RouterImpl should be a concrete implementation of DataSourceRouter<ProjectDataSource>
 * This allows the repository to call template methods on the router.
 */
template<typename RouterImpl>
class MultiSourceProjectRepository : public ProjectRepository {
public:
    explicit MultiSourceProjectRepository(RouterImpl* router, ProjectDTOMapper* dtoMapper) 
        : router(router), mapper(dtoMapper) {
        if (!router) {
            throw std::invalid_argument("Router cannot be null");
        }
        if (!mapper) {
            throw std::invalid_argument("Mapper cannot be null");
        }
    }
    
    virtual ~MultiSourceProjectRepository() = default;
    
    std::string save(const Project& project) override {
        try {
            // Convert entity to DTO using mapper
            std::unique_ptr<dto::ProjectDTO> projectDTO(mapper->toDTO(project));
            
            auto results = router->template executeWrite<std::string>(
                [&projectDTO](ProjectDataSource* ds) {
                    try {
                        return ds->save(*projectDTO);
                    } catch (const std::exception& e) {
                        std::ostringstream oss;
                        oss << "DataSource '" << ds->getName() << "' failed to save project: " << e.what();
                        throw std::runtime_error(oss.str());
                    }
                }
            );
            
            if (results.empty()) {
                throw std::runtime_error("Failed to save project: no datasources available");
            }
            
            return results[0];
        } catch (const std::exception& e) {
            std::ostringstream oss;
            oss << "MultiSourceProjectRepository::save failed for project '" << project.getId() << "': " << e.what();
            throw std::runtime_error(oss.str());
        }
    }
    
    std::optional<Project> findById(const std::string& id) override {
        try {
            auto dtoPtr = router->template executeRead<std::optional<dto::ProjectDTO*>>(
                [&id](ProjectDataSource* ds) {
                    try {
                        return ds->findById(id);
                    } catch (const std::exception& e) {
                        std::ostringstream oss;
                        oss << "DataSource '" << ds->getName() << "' failed to find project: " << e.what();
                        throw std::runtime_error(oss.str());
                    }
                }
            );
            
            if (!dtoPtr || !*dtoPtr) {
                return std::nullopt;
            }
            
            // Convert DTO to entity using the provided mapper
            Project entity = mapper->toEntity(**dtoPtr);
            delete *dtoPtr; // Clean up DTO allocated by data source
            
            return entity;
        } catch (const std::exception& e) {
            std::ostringstream oss;
            oss << "MultiSourceProjectRepository::findById failed for id '" << id << "': " << e.what();
            throw std::runtime_error(oss.str());
        }
    }
    
    std::vector<Project> findAll() override {
        try {
            auto dtoPtrs = router->template executeRead<std::vector<dto::ProjectDTO*>>(
                [](ProjectDataSource* ds) {
                    try {
                        return ds->findAll();
                    } catch (const std::exception& e) {
                        std::ostringstream oss;
                        oss << "DataSource '" << ds->getName() << "' failed to find all projects: " << e.what();
                        throw std::runtime_error(oss.str());
                    }
                }
            );
            
            // Convert DTOs to entities using the provided mapper
            std::vector<Project> projects;
            for (auto* dtoPtr : dtoPtrs) {
                projects.push_back(mapper->toEntity(*dtoPtr));
                delete dtoPtr; // Clean up DTO allocated by data source
            }
            
            return projects;
        } catch (const std::exception& e) {
            std::ostringstream oss;
            oss << "MultiSourceProjectRepository::findAll failed: " << e.what();
            throw std::runtime_error(oss.str());
        }
    }
    
    bool deleteById(const std::string& id) override {
        try {
            auto results = router->template executeWrite<bool>(
                [&id](ProjectDataSource* ds) {
                    try {
                        return ds->deleteById(id);
                    } catch (const std::exception& e) {
                        std::ostringstream oss;
                        oss << "DataSource '" << ds->getName() << "' failed to delete project: " << e.what();
                        throw std::runtime_error(oss.str());
                    }
                }
            );
            
            if (results.empty()) {
                return false;
            }
            
            for (bool result : results) {
                if (result) return true;
            }
            return false;
        } catch (const std::exception& e) {
            std::ostringstream oss;
            oss << "MultiSourceProjectRepository::deleteById failed for id '" << id << "': " << e.what();
            throw std::runtime_error(oss.str());
        }
    }
    
    void update(const Project& project) override {
        try {
            // Convert entity to DTO using mapper
            std::unique_ptr<dto::ProjectDTO> projectDTO(mapper->toDTO(project));
            
            auto results = router->template executeWrite<bool>(
                [&projectDTO](ProjectDataSource* ds) {
                    try {
                        return ds->update(*projectDTO);
                    } catch (const std::exception& e) {
                        std::ostringstream oss;
                        oss << "DataSource '" << ds->getName() << "' failed to update project: " << e.what();
                        throw std::runtime_error(oss.str());
                    }
                }
            );
            
            if (results.empty()) {
                throw std::runtime_error("Failed to update project: no datasources available");
            }
            
            bool anySuccess = false;
            for (bool result : results) {
                if (result) {
                    anySuccess = true;
                    break;
                }
            }
            
            if (!anySuccess) {
                std::ostringstream oss;
                oss << "Failed to update project '" << project.getId() << "': project not found in any datasource";
                throw std::runtime_error(oss.str());
            }
        } catch (const std::exception& e) {
            std::ostringstream oss;
            oss << "MultiSourceProjectRepository::update failed for project '" << project.getId() << "': " << e.what();
            throw std::runtime_error(oss.str());
        }
    }
    
    bool exists(const std::string& id) override {
        try {
            return router->template executeRead<bool>(
                [&id](ProjectDataSource* ds) {
                    try {
                        return ds->exists(id);
                    } catch (const std::exception& e) {
                        std::ostringstream oss;
                        oss << "DataSource '" << ds->getName() << "' failed to check if project exists: " << e.what();
                        throw std::runtime_error(oss.str());
                    }
                }
            );
        } catch (const std::exception& e) {
            return false;
        }
    }

private:
    RouterImpl* router;
    ProjectDTOMapper* mapper;
};

} // namespace repositories
} // namespace plotter

#endif // MULTISOURCE_PROJECT_REPOSITORY_H
