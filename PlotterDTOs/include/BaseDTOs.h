#ifndef BASE_DTOS_H
#define BASE_DTOS_H

namespace plotter {
namespace dto {

/**
 * @brief Base Data Transfer Object for Project data
 * 
 * This is an empty base class that serves as a marker interface.
 * Data source implementations must provide their own DTO structures that inherit from this base.
 * The repository layer uses mappers to convert these opaque DTOs into domain entities.
 * 
 * Example:
 *   struct SqliteProjectDTO : public ProjectDTO {
 *       std::string id;
 *       std::string name;
 *       // ... SQLite-specific fields
 *   };
 */
struct ProjectDTO {
    virtual ~ProjectDTO() = default;
};

/**
 * @brief Base Data Transfer Object for Folder data
 * 
 * This is an empty base class that serves as a marker interface.
 * Data source implementations must provide their own DTO structures that inherit from this base.
 * The repository layer uses mappers to convert these opaque DTOs into domain entities.
 */
struct FolderDTO {
    virtual ~FolderDTO() = default;
};

/**
 * @brief Base Data Transfer Object for Note data
 * 
 * This is an empty base class that serves as a marker interface.
 * Data source implementations must provide their own DTO structures that inherit from this base.
 * The repository layer uses mappers to convert these opaque DTOs into domain entities.
 */
struct NoteDTO {
    virtual ~NoteDTO() = default;
};

} // namespace dto
} // namespace plotter

#endif // BASE_DTOS_H
