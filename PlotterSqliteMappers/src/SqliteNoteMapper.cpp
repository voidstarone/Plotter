#include "plotter_sqlite_mappers/SqliteMappers.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
#include <stdexcept>

namespace plotter {
namespace sqlite_mappers {

// ================ SqliteNoteMapper ================

dto::NoteDTO* SqliteNoteMapper::toDTO(const Note& entity) {
    auto* dto = new sqlite_dtos::SqliteNoteDTO();
    dto->id = entity.getId();
    dto->name = entity.getName();
    dto->path = entity.getPath();
    dto->content = entity.getContent();
    dto->parentFolderId = entity.getParentFolderId();
    dto->createdAt = sqlite_dtos::SqliteDTOUtils::getCurrentTimestamp();
    dto->updatedAt = sqlite_dtos::SqliteDTOUtils::getCurrentTimestamp();
    return dto;
}

Note SqliteNoteMapper::toEntity(const dto::NoteDTO& dto) {
    // Downcast to concrete SqliteNoteDTO
    const auto* sqliteDto = dynamic_cast<const sqlite_dtos::SqliteNoteDTO*>(&dto);
    if (!sqliteDto) {
        throw std::runtime_error("SqliteNoteMapper::toEntity - DTO is not a SqliteNoteDTO");
    }
    
    Note note(sqliteDto->id, sqliteDto->name, sqliteDto->path, sqliteDto->parentFolderId);
    note.setContent(sqliteDto->content);
    return note;
}

} // namespace sqlite_mappers
} // namespace plotter
