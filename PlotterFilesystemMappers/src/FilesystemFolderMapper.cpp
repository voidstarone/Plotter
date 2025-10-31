#include "plotter_filesystem_mappers/FilesystemMappers.h"
#include "plotter_filesystem_dtos/FilesystemDTOs.h"
// Use real entities instead of stubs
#include "Folder.h"

namespace plotter {
namespace filesystem_mappers {

// ================ FilesystemFolderMapper ================

dto::FolderDTO* FilesystemFolderMapper::toDTO(const Folder& entity) {
    auto* dto = new filesystem_dtos::FilesystemFolderDTO();
    dto->id = entity.getId();
    dto->name = entity.getName();
    dto->description = entity.getDescription();
    dto->parentProjectId = entity.getParentProjectId();
    dto->parentFolderId = entity.getParentFolderId();
    dto->noteIds = entity.getNoteIds();
    dto->subfolderIds = entity.getSubfolderIds();
    dto->createdAt = filesystem_dtos::FilesystemDTOUtils::getCurrentTimestamp();
    dto->updatedAt = filesystem_dtos::FilesystemDTOUtils::getCurrentTimestamp();
    // path would be set by the data source layer
    return dto;
}

Folder FilesystemFolderMapper::toEntity(const dto::FolderDTO& dto) {
    // Downcast to concrete FilesystemFolderDTO
    const auto* fsDto = dynamic_cast<const filesystem_dtos::FilesystemFolderDTO*>(&dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemFolderMapper::toEntity - DTO is not a FilesystemFolderDTO");
    }

    Folder folder(
        fsDto->id,
        fsDto->name,
        fsDto->description,
        fsDto->parentProjectId,
        fsDto->parentFolderId
    );

    // Add note IDs
    for (const auto& noteId : fsDto->noteIds) {
        folder.addNoteId(noteId);
    }

    // Add subfolder IDs
    for (const auto& subfolderId : fsDto->subfolderIds) {
        folder.addSubfolderId(subfolderId);
    }

    return folder;
}

} // namespace filesystem_mappers
} // namespace plotter
