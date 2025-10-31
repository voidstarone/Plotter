#include "plotter_sqlite_mappers/SqliteMappers.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
// Use real entities instead of stubs
#include "Project.h"

namespace plotter {
namespace sqlite_mappers {

// ================ SqliteProjectMapper ================

dto::ProjectDTO* SqliteProjectMapper::toDTO(const Project& entity) {
    auto* dto = new sqlite_dtos::SqliteProjectDTO();
    dto->id = entity.getId();
    dto->name = entity.getName();
    dto->description = entity.getDescription();
    dto->folderIds = entity.getFolderIds();
    dto->createdAt = sqlite_dtos::SqliteDTOUtils::getCurrentTimestamp();
    dto->updatedAt = sqlite_dtos::SqliteDTOUtils::getCurrentTimestamp();
    return dto;
}

Project SqliteProjectMapper::toEntity(const dto::ProjectDTO& dto) {
    // Downcast to concrete SqliteProjectDTO
    const auto* sqliteDto = dynamic_cast<const sqlite_dtos::SqliteProjectDTO*>(&dto);
    if (!sqliteDto) {
        throw std::runtime_error("SqliteProjectMapper::toEntity - DTO is not a SqliteProjectDTO");
    }
    
    Project project(sqliteDto->id, sqliteDto->name, sqliteDto->description);
    
    // Add folder IDs from DTO
    for (const auto& folderId : sqliteDto->folderIds) {
        project.addFolderId(folderId);
    }
    
    return project;
}

} // namespace sqlite_mappers
} // namespace plotter
