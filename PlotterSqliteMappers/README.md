# PlotterSqliteMappers

## Purpose

This library provides SQLite-specific implementations of the `EntityDTOMapper` interfaces defined in `PlotterRepositories`. It acts as a bridge layer between domain entities and SQLite data transfer objects.

## Architecture Position

```
┌──────────────────────────────────────┐
│   Domain Layer                       │
│   (PlotterEntities)                  │
│   - Project, Folder, Note            │
└──────────────────────────────────────┘
              ↓ used by
┌──────────────────────────────────────┐
│   Repository Interfaces              │
│   (PlotterRepositories)              │
│   - EntityDTOMapper interfaces       │
│   - MultiSource repositories         │
└──────────────────────────────────────┘
              ↓ implemented by
┌──────────────────────────────────────┐
│   SQLite Mappers (THIS LAYER)        │  ← Bridges entities and DTOs
│   - SqliteProjectMapper              │
│   - SqliteFolderMapper               │
│   - SqliteNoteMapper                 │
└──────────────────────────────────────┘
        ↓ uses                ↓ uses
┌──────────────────┐  ┌──────────────────┐
│  Entities        │  │  SQLite DTOs     │
│  (PlotterEntities│  │ (PlotterSqliteDTOs)│
└──────────────────┘  └──────────────────┘
```

## Why This Layer Exists

In clean architecture, we need to convert between:
- **Domain entities** (Project, Folder, Note) - business logic layer
- **Infrastructure DTOs** (SqliteProjectDTO, etc.) - database layer

The mapper layer solves this by:
1. Depending on BOTH entities (domain) and SQLite DTOs (infrastructure)
2. Implementing the abstract `EntityDTOMapper` interfaces from PlotterRepositories
3. Being injected into repositories, which remain database-agnostic

This allows repositories to work with entities without knowing about database-specific DTO structures.

## Dependencies

This library depends on:
- **PlotterEntities** - For domain entities (Project, Folder, Note)
- **PlotterRepositories** - For EntityDTOMapper interfaces
- **PlotterDTOs** - For base DTO marker interfaces
- **PlotterSqliteDTOs** - For concrete SQLite DTO types

This library has **NO dependencies on**:
- ❌ PlotterSqliteDataSource (datasource implementation)
- ❌ Use Cases
- ❌ SQLite library directly

## Classes

### SqliteProjectMapper
Converts between `Project` entities and `SqliteProjectDTO` objects.

### SqliteFolderMapper
Converts between `Folder` entities and `SqliteFolderDTO` objects.

### SqliteNoteMapper
Converts between `Note` entities and `SqliteNoteDTO` objects.

## Usage

```cpp
#include "plotter_sqlite_mappers/SqliteMappers.h"

// Create mapper instances
plotter::sqlite_mappers::SqliteProjectMapper projectMapper;
plotter::sqlite_mappers::SqliteFolderMapper folderMapper;
plotter::sqlite_mappers::SqliteNoteMapper noteMapper;

// Create repositories with injected mappers
MultiSourceProjectRepository<RouterType> projectRepo(&router, &projectMapper);
MultiSourceFolderRepository<RouterType> folderRepo(&router, &folderMapper);
MultiSourceNoteRepository<RouterType> noteRepo(&router, &noteMapper);

// Repositories are now database-agnostic!
// To use Postgres instead, just create PostgresMappers and inject those
```

## Building

```bash
cd PlotterSqliteMappers
mkdir build && cd build
cmake ..
make
```

## Design Benefits

### ✅ Separation of Concerns
- Entities don't know about databases
- DataSources don't know about entities
- Mappers bridge the gap

### ✅ Dependency Inversion
- Repositories depend on abstract `EntityDTOMapper` interfaces
- Concrete mappers injected at runtime

### ✅ Database Abstraction
- To add Postgres support: create `PlotterPostgresMappers` with same interfaces
- No changes to repositories or entities needed

### ✅ Testability
- Can inject mock mappers for testing
- Can test mappers independently

## Related Projects

- **PlotterEntities** - Domain entity definitions
- **PlotterRepositories** - Repository interfaces and implementations
- **PlotterDTOs** - Base DTO marker interfaces
- **PlotterSqliteDTOs** - SQLite concrete DTO structures
- **PlotterSqliteDataSource** - SQLite database operations
