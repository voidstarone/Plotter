#ifndef PLOTTER_FILESYSTEM_MAPPERS_H
#define PLOTTER_FILESYSTEM_MAPPERS_H

#include "plotter_repositories/EntityDTOMapper.h"
#include "plotter_filesystem_dtos/FilesystemDTOs.h"
// Use real entities instead of stubs
#include "Project.h"
#include "Folder.h"
#include "Note.h"

namespace plotter {
namespace filesystem_mappers {

/**
 * @brief Filesystem-specific implementation of ProjectDTOMapper
 *
 * This mapper bridges between Project entities (domain) and FilesystemProjectDTO (infrastructure).
 * It converts project data stored in filesystem directories with .plotter_project metadata files.
 */
class FilesystemProjectMapper : public repositories::ProjectDTOMapper {
public:
    /**
     * @brief Convert a Project entity to a FilesystemProjectDTO
     *
     * @param entity The domain entity
     * @return Pointer to FilesystemProjectDTO (caller owns memory)
     */
    dto::ProjectDTO* toDTO(const Project& entity) override;

    /**
     * @brief Convert a FilesystemProjectDTO to a Project entity
     *
     * @param dto The DTO from datasource (must be FilesystemProjectDTO)
     * @return Domain entity
     */
    Project toEntity(const dto::ProjectDTO& dto) override;
};

/**
 * @brief Filesystem-specific implementation of FolderDTOMapper
 *
 * Converts between Folder entities and FilesystemFolderDTO objects.
 */
class FilesystemFolderMapper : public repositories::FolderDTOMapper {
public:
    dto::FolderDTO* toDTO(const Folder& entity) override;
    Folder toEntity(const dto::FolderDTO& dto) override;
};

/**
 * @brief Filesystem-specific implementation of NoteDTOMapper
 *
 * Converts between Note entities and FilesystemNoteDTO objects.
 */
class FilesystemNoteMapper : public repositories::NoteDTOMapper {
public:
    dto::NoteDTO* toDTO(const Note& entity) override;
    Note toEntity(const dto::NoteDTO& dto) override;
};

} // namespace filesystem_mappers
} // namespace plotter

#endif // PLOTTER_FILESYSTEM_MAPPERS_H
