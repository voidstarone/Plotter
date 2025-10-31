#include "plotter_filesystem_mappers/FilesystemMappers.h"
#include "plotter_filesystem_dtos/FilesystemDTOs.h"
// Use real entities instead of stubs
#include "Project.h"

namespace plotter {
namespace filesystem_mappers {

// ================ FilesystemProjectMapper ================

dto::ProjectDTO* FilesystemProjectMapper::toDTO(const Project& entity) {
    auto* dto = new filesystem_dtos::FilesystemProjectDTO();
    dto->id = entity.getId();
    dto->name = entity.getName();
    dto->description = entity.getDescription();
    dto->folderIds = entity.getFolderIds();
    dto->createdAt = filesystem_dtos::FilesystemDTOUtils::getCurrentTimestamp();
    dto->updatedAt = filesystem_dtos::FilesystemDTOUtils::getCurrentTimestamp();
    // rootPath would be set by the data source layer
    return dto;
}

Project FilesystemProjectMapper::toEntity(const dto::ProjectDTO& dto) {
    // Downcast to concrete FilesystemProjectDTO
    const auto* fsDto = dynamic_cast<const filesystem_dtos::FilesystemProjectDTO*>(&dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemProjectMapper::toEntity - DTO is not a FilesystemProjectDTO");
    }

    Project project(fsDto->id, fsDto->name, fsDto->description);

    // Add folder IDs from DTO
    for (const auto& folderId : fsDto->folderIds) {
        project.addFolderId(folderId);
    }

    return project;
}

} // namespace filesystem_mappers
} // namespace plotter
