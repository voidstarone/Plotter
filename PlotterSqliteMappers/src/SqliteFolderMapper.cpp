#include "plotter_sqlite_mappers/SqliteMappers.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
// Use real entities instead of stubs
#include "Folder.h"

namespace plotter {
namespace sqlite_mappers {

// ================ SqliteFolderMapper ================

dto::FolderDTO* SqliteFolderMapper::toDTO(const Folder& entity) {
    auto* dto = new sqlite_dtos::SqliteFolderDTO();
    dto->id = entity.getId();
    dto->name = entity.getName();
    dto->description = entity.getDescription();
    dto->parentProjectId = entity.getParentProjectId();
    dto->parentFolderId = entity.getParentFolderId();
    dto->noteIds = entity.getNoteIds();
    dto->subfolderIds = entity.getSubfolderIds();
    dto->createdAt = sqlite_dtos::SqliteDTOUtils::getCurrentTimestamp();
    dto->updatedAt = sqlite_dtos::SqliteDTOUtils::getCurrentTimestamp();
    return dto;
}

Folder SqliteFolderMapper::toEntity(const dto::FolderDTO& dto) {
    // Downcast to concrete SqliteFolderDTO
    const auto* sqliteDto = dynamic_cast<const sqlite_dtos::SqliteFolderDTO*>(&dto);
    if (!sqliteDto) {
        throw std::runtime_error("SqliteFolderMapper::toEntity - DTO is not a SqliteFolderDTO");
    }
    
    Folder folder(sqliteDto->id, sqliteDto->name, sqliteDto->description,
                  sqliteDto->parentProjectId, sqliteDto->parentFolderId);
    
    // Add note IDs from DTO
    for (const auto& noteId : sqliteDto->noteIds) {
        folder.addNoteId(noteId);
    }
    
    // Add subfolder IDs from DTO
    for (const auto& subfolderId : sqliteDto->subfolderIds) {
        folder.addSubfolderId(subfolderId);
    }
    
    return folder;
}

} // namespace sqlite_mappers
} // namespace plotter
