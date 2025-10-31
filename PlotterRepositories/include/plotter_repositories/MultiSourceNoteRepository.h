#ifndef MULTISOURCE_NOTE_REPOSITORY_H
#define MULTISOURCE_NOTE_REPOSITORY_H

#include "repositories/NoteRepository.h"
#include "plotter_repositories/NoteDataSource.h"
#include "plotter_repositories/DataSourceRouter.h"
#include "plotter_repositories/EntityDTOMapper.h"
#include <memory>
#include <stdexcept>
#include <sstream>

namespace plotter {
namespace repositories {

/**
 * @brief Multi-source implementation of NoteRepository
 * 
 * This class implements the NoteRepository interface from PlotterUseCases
 * using a DataSourceRouter to intelligently route operations across multiple
 * datasources (cache, database, filesystem, etc.)
 * 
 * @tparam RouterType The concrete type of DataSourceRouter (e.g., SimpleDataSourceRouter<NoteDataSource>)
 */
template<typename RouterType>
class MultiSourceNoteRepository : public NoteRepository {
private:
    RouterType* router;
    NoteDTOMapper* mapper;  // Mapper provided by datasource implementation
    
public:
    /**
     * @brief Construct a multi-source note repository
     * 
     * @param router Pointer to the datasource router (ownership NOT transferred)
     * @param dtoMapper Pointer to the DTO mapper for entity/DTO conversion (ownership NOT transferred)
     */
    explicit MultiSourceNoteRepository(RouterType* router, NoteDTOMapper* dtoMapper);
    
    virtual ~MultiSourceNoteRepository() = default;
    
    // Implement NoteRepository interface
    std::string save(const Note& note) override;
    std::optional<Note> findById(const std::string& id) override;
    std::vector<Note> findAll() override;
    std::vector<Note> findByParentFolderId(const std::string& parentFolderId) override;
    std::vector<Note> search(const std::string& searchTerm) override;
    bool deleteById(const std::string& id) override;
    void update(const Note& note) override;
    bool exists(const std::string& id) override;
};

// Template implementation (must be in header)
template<typename RouterType>
MultiSourceNoteRepository<RouterType>::MultiSourceNoteRepository(RouterType* router, NoteDTOMapper* dtoMapper)
    : router(router), mapper(dtoMapper) {
    if (!router) {
        throw std::invalid_argument("Router cannot be null");
    }
    if (!mapper) {
        throw std::invalid_argument("DTO Mapper cannot be null");
    }
}

template<typename RouterType>
std::string MultiSourceNoteRepository<RouterType>::save(const Note& note) {
    try {
        // Convert entity to DTO using the provided mapper (database-agnostic)
        std::unique_ptr<dto::NoteDTO> noteDTO(mapper->toDTO(note));
        
        auto results = router->template executeWrite<std::string>(
            [&noteDTO](NoteDataSource* ds) {
                try {
                    return ds->save(*noteDTO);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to save note: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        if (results.empty()) {
            throw std::runtime_error("Failed to save note: no datasources available");
        }
        
        return results[0];
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceNoteRepository::save failed for note '" << note.getId() << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
std::optional<Note> MultiSourceNoteRepository<RouterType>::findById(const std::string& id) {
    try {
        auto dtoPtr = router->template executeRead<std::optional<dto::NoteDTO*>>(
            [&id](NoteDataSource* ds) {
                try {
                    return ds->findById(id);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to find note: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        // Convert DTO to entity if found using the provided mapper
        if (dtoPtr && *dtoPtr) {
            Note entity = mapper->toEntity(**dtoPtr);
            delete *dtoPtr; // Clean up DTO allocated by data source
            return entity;
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceNoteRepository::findById failed for id '" << id << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
std::vector<Note> MultiSourceNoteRepository<RouterType>::findAll() {
    try {
        auto dtoPtrs = router->template executeRead<std::vector<dto::NoteDTO*>>(
            [](NoteDataSource* ds) {
                try {
                    return ds->findAll();
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to find all notes: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        // Convert DTOs to entities using the provided mapper
        std::vector<Note> notes;
        for (auto* dtoPtr : dtoPtrs) {
            notes.push_back(mapper->toEntity(*dtoPtr));
            delete dtoPtr; // Clean up DTO allocated by data source
        }
        
        return notes;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceNoteRepository::findAll failed: " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
std::vector<Note> MultiSourceNoteRepository<RouterType>::findByParentFolderId(const std::string& parentFolderId) {
    try {
        auto dtoPtrs = router->template executeRead<std::vector<dto::NoteDTO*>>(
            [&parentFolderId](NoteDataSource* ds) {
                try {
                    return ds->findByParentFolderId(parentFolderId);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to find notes by folder: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        // Convert DTOs to entities using the provided mapper
        std::vector<Note> notes;
        for (auto* dtoPtr : dtoPtrs) {
            notes.push_back(mapper->toEntity(*dtoPtr));
            delete dtoPtr; // Clean up DTO allocated by data source
        }
        
        return notes;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceNoteRepository::findByParentFolderId failed for folderId '" << parentFolderId << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
std::vector<Note> MultiSourceNoteRepository<RouterType>::search(const std::string& searchTerm) {
    try {
        auto dtoPtrs = router->template executeRead<std::vector<dto::NoteDTO*>>(
            [&searchTerm](NoteDataSource* ds) {
                try {
                    return ds->search(searchTerm);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to search notes: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        // Convert DTOs to entities using the provided mapper
        std::vector<Note> notes;
        for (auto* dtoPtr : dtoPtrs) {
            notes.push_back(mapper->toEntity(*dtoPtr));
            delete dtoPtr; // Clean up DTO allocated by data source
        }
        
        return notes;
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceNoteRepository::search failed for term '" << searchTerm << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
bool MultiSourceNoteRepository<RouterType>::deleteById(const std::string& id) {
    try {
        auto results = router->template executeWrite<bool>(
            [&id](NoteDataSource* ds) {
                try {
                    return ds->deleteById(id);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to delete note: " << e.what();
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
        oss << "MultiSourceNoteRepository::deleteById failed for id '" << id << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
void MultiSourceNoteRepository<RouterType>::update(const Note& note) {
    try {
        // Convert entity to DTO using mapper
        std::unique_ptr<dto::NoteDTO> noteDTO(mapper->toDTO(note));
        
        auto results = router->template executeWrite<bool>(
            [&noteDTO](NoteDataSource* ds) {
                try {
                    return ds->update(*noteDTO);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to update note: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
        
        if (results.empty()) {
            throw std::runtime_error("Failed to update note: no datasources available");
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
            oss << "Failed to update note '" << note.getId() << "': note not found in any datasource";
            throw std::runtime_error(oss.str());
        }
    } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "MultiSourceNoteRepository::update failed for note '" << note.getId() << "': " << e.what();
        throw std::runtime_error(oss.str());
    }
}

template<typename RouterType>
bool MultiSourceNoteRepository<RouterType>::exists(const std::string& id) {
    try {
        return router->template executeRead<bool>(
            [&id](NoteDataSource* ds) {
                try {
                    return ds->exists(id);
                } catch (const std::exception& e) {
                    std::ostringstream oss;
                    oss << "DataSource '" << ds->getName() << "' failed to check if note exists: " << e.what();
                    throw std::runtime_error(oss.str());
                }
            }
        );
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace repositories
} // namespace plotter

#endif // MULTISOURCE_NOTE_REPOSITORY_H