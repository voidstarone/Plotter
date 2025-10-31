#ifndef ENTITY_DTO_MAPPER_H
#define ENTITY_DTO_MAPPER_H

#include "BaseDTOs.h"
#include "Project.h"
#include "Folder.h"
#include "Note.h"
#include <vector>
#include <string>

namespace plotter {
namespace repositories {

/**
 * @brief Abstract interface for mapping between entities and DTOs
 * 
 * This interface allows repositories to convert between domain entities
 * and data transfer objects without knowing the concrete DTO implementation.
 * Each datasource provides its own concrete mapper implementation.
 * 
 * This maintains clean architecture by:
 * - Repositories don't know about specific DTO implementations (SQLite, Postgres, etc.)
 * - DataSources are responsible for their own DTO structure
 * - The repository just needs the mapper interface
 * 
 * Important: DTOs should contain all data needed for entity reconstruction,
 * including related entity IDs (e.g., folderIds for Project, noteIds for Folder).
 * The datasource is responsible for populating these fields when fetching from storage.
 */
class ProjectDTOMapper {
public:
    virtual ~ProjectDTOMapper() = default;
    
    /**
     * @brief Convert a Project entity to a DTO
     * 
     * @param entity The domain entity
     * @return Pointer to DTO (caller owns memory)
     */
    virtual dto::ProjectDTO* toDTO(const Project& entity) = 0;
    
    /**
     * @brief Convert a DTO to a Project entity
     * 
     * The DTO should contain all necessary data including folder IDs.
     * 
     * @param dto The DTO from datasource (must include all project data)
     * @return Domain entity
     */
    virtual Project toEntity(const dto::ProjectDTO& dto) = 0;
};

/**
 * @brief Abstract interface for Folder entity/DTO mapping
 */
class FolderDTOMapper {
public:
    virtual ~FolderDTOMapper() = default;
    
    virtual dto::FolderDTO* toDTO(const Folder& entity) = 0;
    
    /**
     * @brief Convert a DTO to a Folder entity
     * 
     * The DTO should contain all necessary data including note and subfolder IDs.
     * 
     * @param dto The DTO from datasource (must include all folder data)
     * @return Domain entity
     */
    virtual Folder toEntity(const dto::FolderDTO& dto) = 0;
};

/**
 * @brief Abstract interface for Note entity/DTO mapping
 */
class NoteDTOMapper {
public:
    virtual ~NoteDTOMapper() = default;
    
    virtual dto::NoteDTO* toDTO(const Note& entity) = 0;
    
    virtual Note toEntity(const dto::NoteDTO& dto) = 0;
};

} // namespace repositories
} // namespace plotter

#endif // ENTITY_DTO_MAPPER_H
