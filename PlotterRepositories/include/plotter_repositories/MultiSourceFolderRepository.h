#ifndef MULTISOURCE_FOLDER_REPOSITORY_H
#define MULTISOURCE_FOLDER_REPOSITORY_H

#include "repositories/FolderRepository.h"
#include "plotter_repositories/FolderDataSource.h"
#include "plotter_repositories/DataSourceRouter.h"
#include "plotter_repositories/EntityDTOMapper.h"
#include <memory>
#include <stdexcept>
#include <sstream>

namespace plotter {
namespace repositories {

/**
 * @brief Multi-source implementation of FolderRepository
 * 
 * This class implements the FolderRepository interface from PlotterUseCases
 * using a DataSourceRouter to intelligently route operations across multiple
 * datasources (cache, database, filesystem, etc.)
 * 
 * @tparam RouterType The concrete type of DataSourceRouter (e.g., SimpleDataSourceRouter<FolderDataSource>)
 */
template<typename RouterType>
class MultiSourceFolderRepository : public FolderRepository {
private:
    RouterType* router;
    FolderDTOMapper* mapper;
    
public:
    /**
     * @brief Construct a multi-source folder repository
     * 
     * @param router Pointer to the datasource router (ownership NOT transferred)
     * @param dtoMapper Pointer to the DTO mapper (ownership NOT transferred)
     */
    explicit MultiSourceFolderRepository(RouterType* router, FolderDTOMapper* dtoMapper);
    
    virtual ~MultiSourceFolderRepository() = default;
    
    // Implement FolderRepository interface
    std::string save(const Folder& folder) override;
    std::optional<Folder> findById(const std::string& id) override;
    std::vector<Folder> findAll() override;
    std::vector<Folder> findByParentProjectId(const std::string& parentProjectId) override;
    std::vector<Folder> findByParentFolderId(const std::string& parentFolderId) override;
    bool deleteById(const std::string& id) override;
    void update(const Folder& folder) override;
    bool exists(const std::string& id) override;
};

// Template implementation (must be in header)
template<typename RouterType>
MultiSourceFolderRepository<RouterType>::MultiSourceFolderRepository(RouterType* router, FolderDTOMapper* dtoMapper)
    : router(router), mapper(dtoMapper) {
    if (!router) {
        throw std::invalid_argument("Router cannot be null");
    }
    if (!mapper) {
        throw std::invalid_argument("Mapper cannot be null");
    }
}

template<typename RouterType>
std::string MultiSourceFolderRepository<RouterType>::save(const Folder& folder) {
    try {
        // Convert entity to DTO using mapper
        std::unique_ptr<dto::FolderDTO> folderDTO(mapper->toDTO(folder));
        
        return router->template executeRead<std::string>(
            [&folderDTO](FolderDataSource* ds) {
                try {
                    return ds->save(*folderDTO);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to save folder: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceFolderRepository::save failed for folder '" << folder.getId() << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
std::optional<Folder> MultiSourceFolderRepository<RouterType>::findById(const std::string& id) {
    try {
        auto dtoPtr = router->template executeRead<std::optional<dto::FolderDTO*>>(
            [&id](FolderDataSource* ds) {
                try {
                    return ds->findById(id);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to find folder: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        if (!dtoPtr || !*dtoPtr) {
            return std::nullopt;
        }
        
        // Convert DTO to entity using the provided mapper
        Folder entity = mapper->toEntity(**dtoPtr);
        delete *dtoPtr; // Clean up DTO allocated by data source
        
        return entity;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceFolderRepository::findById failed for id '" << id << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
std::vector<Folder> MultiSourceFolderRepository<RouterType>::findAll() {
    try {
        auto dtoPtrs = router->template executeRead<std::vector<dto::FolderDTO*>>(
            [](FolderDataSource* ds) {
                try {
                    return ds->findAll();
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to find all folders: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        // Convert DTOs to entities using the provided mapper
        std::vector<Folder> folders;
        for (auto* dtoPtr : dtoPtrs) {
            folders.push_back(mapper->toEntity(*dtoPtr));
            delete dtoPtr; // Clean up DTO allocated by data source
        }
        
        return folders;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceFolderRepository::findAll failed: " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
std::vector<Folder> MultiSourceFolderRepository<RouterType>::findByParentProjectId(const std::string& parentProjectId) {
    try {
        auto dtoPtrs = router->template executeRead<std::vector<dto::FolderDTO*>>(
            [&parentProjectId](FolderDataSource* ds) {
                try {
                    return ds->findByProjectId(parentProjectId);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to find folders by project: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        // Convert DTOs to entities using the provided mapper
        std::vector<Folder> folders;
        for (auto* dtoPtr : dtoPtrs) {
            folders.push_back(mapper->toEntity(*dtoPtr));
            delete dtoPtr; // Clean up DTO allocated by data source
        }
        
        return folders;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceFolderRepository::findByParentProjectId failed for projectId '" << parentProjectId << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
std::vector<Folder> MultiSourceFolderRepository<RouterType>::findByParentFolderId(const std::string& parentFolderId) {
    try {
        auto dtoPtrs = router->template executeRead<std::vector<dto::FolderDTO*>>(
            [&parentFolderId](FolderDataSource* ds) {
                try {
                    return ds->findByParentFolderId(parentFolderId);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to find folders by parent: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        // Convert DTOs to entities using the provided mapper
        std::vector<Folder> folders;
        for (auto* dtoPtr : dtoPtrs) {
            folders.push_back(mapper->toEntity(*dtoPtr));
            delete dtoPtr; // Clean up DTO allocated by data source
        }
        
        return folders;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceFolderRepository::findByParentFolderId failed for folderId '" << parentFolderId << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
bool MultiSourceFolderRepository<RouterType>::deleteById(const std::string& id) {
    try {
        auto results = router->template executeWrite<bool>(
            [&id](FolderDataSource* ds) {
                try {
                    return ds->deleteById(id);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to delete folder: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        for (bool result : results) {
            if (result) return true;
        }
        return false;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceFolderRepository::deleteById failed for id '" << id << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
void MultiSourceFolderRepository<RouterType>::update(const Folder& folder) {
    try {
        // Convert entity to DTO using mapper
        std::unique_ptr<dto::FolderDTO> folderDTO(mapper->toDTO(folder));
        
        auto results = router->template executeWrite<bool>(
            [&folderDTO](FolderDataSource* ds) {
                try {
                    return ds->update(*folderDTO);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to update folder: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        bool anySuccess = false;
        for (bool result : results) {
            if (result) anySuccess = true;
        }
        
        if (!anySuccess) {
            throw std::runtime_error("Failed to update folder in any datasource");
        }
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceFolderRepository::update failed for folder '" << folder.getId() << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
bool MultiSourceFolderRepository<RouterType>::exists(const std::string& id) {
    try {
        return router->template executeRead<bool>(
            [&id](FolderDataSource* ds) {
                try {
                    return ds->exists(id);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to check folder existence: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceFolderRepository::exists failed for id '" << id << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

} // namespace repositories
} // namespace plotter

#endif // MULTISOURCE_FOLDER_REPOSITORY_H