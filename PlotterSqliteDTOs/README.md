# PlotterSqliteDTOs

## Purpose

This library provides SQLite-specific Data Transfer Object (DTO) definitions that extend the base DTO marker interfaces from `PlotterDTOs`.

## Architecture Position

```
┌──────────────────────────────────────┐
│   Base DTOs (Marker Interfaces)     │
│   (PlotterDTOs)                      │
│   - ProjectDTO (empty)               │
│   - FolderDTO (empty)                │
│   - NoteDTO (empty)                  │
└──────────────────────────────────────┘
              ↑
┌──────────────────────────────────────┐
│   SQLite DTOs (THIS LAYER)           │  ← Concrete DTO implementations
│   - SqliteProjectDTO                 │
│   - SqliteFolderDTO                  │
│   - SqliteNoteDTO                    │
│   - SqliteDTOUtils                   │
└──────────────────────────────────────┘
```

## Why This Layer Exists

In the opaque DTO pattern:
- **Base DTOs** are empty marker interfaces (no fields)
- **Concrete DTOs** (like SqliteProjectDTO) inherit from base DTOs and add database-specific fields
- This allows repositories to work with `dto::ProjectDTO*` pointers without knowing the concrete type
- Different databases can have different DTO structures (SQLite vs Postgres vs MongoDB)

## Dependencies

This library depends on:
- **PlotterDTOs** - For base DTO marker interfaces

This library has **NO dependencies on**:
- ❌ Entities (domain layer)
- ❌ Repositories
- ❌ Use Cases
- ❌ SQLite datasource implementation

## Classes

### SqliteProjectDTO
Concrete DTO for Project data as stored in SQLite database.

**Fields:**
- `id`, `name`, `description`
- `createdAt`, `updatedAt` (timestamps)
- `folderIds` (child folder IDs)

### SqliteFolderDTO
Concrete DTO for Folder data as stored in SQLite database.

**Fields:**
- `id`, `name`, `description`
- `parentProjectId`, `parentFolderId`
- `createdAt`, `updatedAt` (timestamps)
- `noteIds`, `subfolderIds` (child IDs)

### SqliteNoteDTO
Concrete DTO for Note data as stored in SQLite database.

**Fields:**
- `id`, `name`, `path`, `content`
- `parentFolderId`
- `createdAt`, `updatedAt` (timestamps)

### SqliteDTOUtils
Utility class for SQLite DTO operations.

**Methods:**
- `getCurrentTimestamp()` - Returns current time as Unix timestamp in milliseconds

## Usage

```cpp
#include "plotter_sqlite_dtos/SqliteDTOs.h"

// Create a DTO
auto dto = new plotter::sqlite_dtos::SqliteProjectDTO();
dto->id = "proj-1";
dto->name = "My Project";
dto->description = "A test project";
dto->createdAt = plotter::sqlite_dtos::SqliteDTOUtils::getCurrentTimestamp();
dto->updatedAt = dto->createdAt;

// Can be used polymorphically as base type
plotter::dto::ProjectDTO* baseDto = dto;
```

## Building

```bash
cd PlotterSqliteDTOs
mkdir build && cd build
cmake ..
make
```

### Build Options

```bash
# Build with tests
cmake .. -DBUILD_TESTS=ON
make
./tests/test_sqlite_dtos
```

## Design Benefits

### ✅ Database Independence
- Different databases can define their own DTO structures
- Repositories work with base DTO pointers (opaque)
- No coupling between database implementations

### ✅ Clean Separation
- DTOs are pure data structures (no logic)
- No dependency on domain entities
- Infrastructure layer component

### ✅ Type Safety
- Compile-time type checking
- Downcasting handled in mapper layer

## Related Projects

- **PlotterDTOs** - Base DTO marker interfaces
- **PlotterSqliteDTOs** - SQLite concrete DTOs (this library)
- **PlotterSqliteMappers** - Converts between entities and SQLite DTOs
- **PlotterSqliteDataSource** - SQLite database operations using these DTOs
