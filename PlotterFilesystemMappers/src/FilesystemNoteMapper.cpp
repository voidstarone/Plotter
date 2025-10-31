#include "plotter_filesystem_mappers/FilesystemMappers.h"
#include "plotter_filesystem_dtos/FilesystemDTOs.h"
// Use real entities instead of stubs
#include "Note.h"

namespace plotter {
namespace filesystem_mappers {

// ================ FilesystemNoteMapper ================

dto::NoteDTO* FilesystemNoteMapper::toDTO(const Note& entity) {
    auto* dto = new filesystem_dtos::FilesystemNoteDTO();
    dto->id = entity.getId();
    dto->name = entity.getName();
    dto->path = entity.getPath();
    dto->parentFolderId = entity.getParentFolderId();
    dto->createdAt = filesystem_dtos::FilesystemDTOUtils::getCurrentTimestamp();
    dto->updatedAt = filesystem_dtos::FilesystemDTOUtils::getCurrentTimestamp();
    // content would be read from file by the data source layer
    return dto;
}

Note FilesystemNoteMapper::toEntity(const dto::NoteDTO& dto) {
    // Downcast to concrete FilesystemNoteDTO
    const auto* fsDto = dynamic_cast<const filesystem_dtos::FilesystemNoteDTO*>(&dto);
    if (!fsDto) {
        throw std::runtime_error("FilesystemNoteMapper::toEntity - DTO is not a FilesystemNoteDTO");
    }

    Note note(
        fsDto->id,
        fsDto->name,
        fsDto->path,
        fsDto->parentFolderId
    );

    return note;
}

} // namespace filesystem_mappers
} // namespace plotter
