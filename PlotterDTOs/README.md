# PlotterDTOs

This package defines the base Data Transfer Object (DTO) marker interfaces for the Plotter application.

## Purpose

- Provides **empty marker interfaces** (no fields) that all data source DTOs must inherit from
- Enables polymorphic DTO handling in repositories without coupling to specific implementations
- Maintains clean architecture by allowing repositories to work with base DTO pointers

## Architecture

```
┌────────────────────────────────────────┐
│  PlotterDTOs (Marker Interfaces)      │
│  - ProjectDTO (empty interface)       │
│  - FolderDTO (empty interface)        │
│  - NoteDTO (empty interface)          │
└────────────────────────────────────────┘
                ↑
                │ inherits from
                │
┌────────────────────────────────────────┐
│  Concrete DTO Implementations          │
│                                        │
│  PlotterSqliteDTOs:                    │
│  - SqliteProjectDTO : ProjectDTO       │
│  - SqliteFolderDTO : FolderDTO         │
│  - SqliteNoteDTO : NoteDTO             │
│                                        │
│  PlotterPostgresDTOs (future):         │
│  - PostgresProjectDTO : ProjectDTO     │
│  - etc.                                │
└────────────────────────────────────────┘
```

## Why Marker Interfaces?

The **Opaque DTO Pattern** used here provides:

1. **Database Independence**: Repositories work with `dto::ProjectDTO*` pointers without knowing if it's SQLite, Postgres, MongoDB, etc.
2. **No Coupling**: Repositories don't include database-specific headers
3. **Flexibility**: Each database can define its own DTO structure with different fields
4. **Type Safety**: Downcasting is handled safely in the mapper layer

## Usage

### Defining Concrete DTOs

Data source implementations create concrete DTOs that inherit from the marker interfaces:

```cpp
#include "BaseDTOs.h"

namespace plotter::sqlite_dtos {
    // Concrete DTO with actual fields
    struct SqliteProjectDTO : public plotter::dto::ProjectDTO {
        std::string id;
        std::string name;
        std::string description;
        long long createdAt;
        long long updatedAt;
        std::vector<std::string> folderIds;
    };
}
```

### Using in Repositories

Repositories work with base DTO pointers:

```cpp
// Repository only knows about base DTO interface
dto::ProjectDTO* baseDto = getFromDataSource();

// Mapper handles downcasting to concrete type
Project entity = mapper->toEntity(*baseDto);
```

## Structure

```
PlotterDTOs/
├── include/
│   └── BaseDTOs.h          # ProjectDTO, FolderDTO, NoteDTO interfaces
└── CMakeLists.txt
```

## Building

This is a header-only interface library:

```bash
cd PlotterDTOs
mkdir build && cd build
cmake ..
```

## Related Libraries

- **PlotterSqliteDTOs** - Concrete SQLite DTO implementations
- **PlotterSqliteMappers** - Converts between entities and SQLite DTOs
- **PlotterRepositories** - Uses these marker interfaces for database abstraction
