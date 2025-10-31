#ifndef PLOTTER_SQLITE_MAPPERS_H
#define PLOTTER_SQLITE_MAPPERS_H

#include "plotter_repositories/EntityDTOMapper.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
// Use real entities instead of stubs
#include "Project.h"
#include "Folder.h"
#include "Note.h"

namespace plotter {
namespace sqlite_mappers {

/**
 * @brief SQLite-specific implementation of ProjectDTOMapper
 * 
 * This mapper bridges between Project entities (domain) and SqliteProjectDTO (infrastructure).
 * It lives in a separate layer between repositories and datasources.
 * 
 * Architecture:
 *   Entities (domain) ← Mapper Layer → SQLite DTOs (infrastructure)
 */
class SqliteProjectMapper : public repositories::ProjectDTOMapper {
public:
    /**
     * @brief Convert a Project entity to a SqliteProjectDTO
     * 
     * @param entity The domain entity
     * @return Pointer to SqliteProjectDTO (caller owns memory)
     */
    dto::ProjectDTO* toDTO(const Project& entity) override;
    
    /**
     * @brief Convert a SqliteProjectDTO to a Project entity
     * 
     * @param dto The DTO from datasource (must be SqliteProjectDTO)
     * @return Domain entity
     */
    Project toEntity(const dto::ProjectDTO& dto) override;
};

/**
 * @brief SQLite-specific implementation of FolderDTOMapper
 * 
 * Converts between Folder entities and SqliteFolderDTO objects.
 */
class SqliteFolderMapper : public repositories::FolderDTOMapper {
public:
    dto::FolderDTO* toDTO(const Folder& entity) override;
    Folder toEntity(const dto::FolderDTO& dto) override;
};

/**
 * @brief SQLite-specific implementation of NoteDTOMapper
 * 
 * Converts between Note entities and SqliteNoteDTO objects.
 */
class SqliteNoteMapper : public repositories::NoteDTOMapper {
public:
    dto::NoteDTO* toDTO(const Note& entity) override;
    Note toEntity(const dto::NoteDTO& dto) override;
};

} // namespace sqlite_mappers
} // namespace plotter

#endif // PLOTTER_SQLITE_MAPPERS_H
