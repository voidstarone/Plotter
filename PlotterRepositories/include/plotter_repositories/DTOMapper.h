#ifndef PLOTTER_REPOSITORIES_DTO_MAPPER_H
#define PLOTTER_REPOSITORIES_DTO_MAPPER_H

#include "BaseDTOs.h"
#include "Project.h"
#include "Folder.h"
#include "Note.h"
#include "NoteStorage.h"
#include <vector>
#include <string>
#include <memory>

namespace plotter {
namespace repositories {

/**
 * @brief Interface for mapping between opaque DTOs and domain entities
 * 
 * Each data source implementation provides its own mapper that knows how to convert
 * its specific DTO structures into domain entities and vice versa.
 * 
 * This allows the repository to work with any data source without knowing the
 * concrete structure of the DTOs.
 */
template<typename EntityType, typename DTOType>
class DTOMapper {
public:
    virtual ~DTOMapper() = default;
    
    /**
     * @brief Convert a domain entity to a DTO
     * 
     * @param entity The domain entity
     * @return DTO for data source storage (allocated on heap, caller owns)
     */
    virtual DTOType* toDTO(const EntityType& entity) = 0;
    
    /**
     * @brief Convert a DTO to a domain entity
     * 
     * @param dto The DTO from data source
     * @return Domain entity
     */
    virtual EntityType toEntity(const DTOType& dto) = 0;
};

/**
 * @brief Mapper interface for Project entities
 */
class ProjectDTOMapper : public DTOMapper<Project, dto::ProjectDTO> {
public:
    virtual ~ProjectDTOMapper() = default;
    
    /**
     * @brief Convert a Project entity to a DTO
     * @param entity The Project domain entity
     * @return DTO for data source storage
     */
    virtual dto::ProjectDTO* toDTO(const Project& entity) override = 0;
    
    /**
     * @brief Convert a DTO to a Project entity
     * @param dto The DTO from data source
     * @param folderIds Vector of folder IDs belonging to this project (queried separately)
     * @return Project domain entity
     */
    virtual Project toEntity(const dto::ProjectDTO& dto) override = 0;
    
    /**
     * @brief Convert a DTO to a Project entity with folder IDs
     * @param dto The DTO from data source
     * @param folderIds Vector of folder IDs belonging to this project
     * @return Project domain entity
     */
    virtual Project toEntityWithFolders(const dto::ProjectDTO& dto, 
                                        const std::vector<std::string>& folderIds) = 0;
};

/**
 * @brief Mapper interface for Folder entities
 */
class FolderDTOMapper : public DTOMapper<Folder, dto::FolderDTO> {
public:
    virtual ~FolderDTOMapper() = default;
    
    /**
     * @brief Convert a Folder entity to a DTO
     * @param entity The Folder domain entity
     * @return DTO for data source storage
     */
    virtual dto::FolderDTO* toDTO(const Folder& entity) override = 0;
    
    /**
     * @brief Convert a DTO to a Folder entity
     * @param dto The DTO from data source
     * @return Folder domain entity
     */
    virtual Folder toEntity(const dto::FolderDTO& dto) override = 0;
    
    /**
     * @brief Convert a DTO to a Folder entity with children
     * @param dto The DTO from data source
     * @param noteIds Vector of note IDs in this folder
     * @param subfolderIds Vector of subfolder IDs in this folder
     * @return Folder domain entity
     */
    virtual Folder toEntityWithChildren(const dto::FolderDTO& dto,
                                        const std::vector<std::string>& noteIds,
                                        const std::vector<std::string>& subfolderIds) = 0;
};

/**
 * @brief Mapper interface for Note entities
 */
class NoteDTOMapper : public DTOMapper<Note, dto::NoteDTO> {
public:
    virtual ~NoteDTOMapper() = default;
    
    /**
     * @brief Convert a Note entity to a DTO
     * @param entity The Note domain entity
     * @return DTO for data source storage
     */
    virtual dto::NoteDTO* toDTO(const Note& entity) override = 0;
    
    /**
     * @brief Convert a DTO to a Note entity
     * @param dto The DTO from data source
     * @return Note domain entity
     */
    virtual Note toEntity(const dto::NoteDTO& dto) override = 0;
    
    /**
     * @brief Convert a DTO to a Note entity with storage backend
     * @param dto The DTO from data source
     * @param storage Shared pointer to NoteStorage for lazy content loading
     * @return Note domain entity
     */
    virtual Note toEntityWithStorage(const dto::NoteDTO& dto, 
                                     std::shared_ptr<NoteStorage> storage) = 0;
};

} // namespace repositories
} // namespace plotter

#endif // PLOTTER_REPOSITORIES_DTO_MAPPER_H
