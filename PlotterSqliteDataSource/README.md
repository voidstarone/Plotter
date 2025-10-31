# Plotter SQLite DataSource

A production-ready SQLite implementation of the PlotterRepositories datasource interfaces, following Clean Architecture principles with proper relational database design.

## Overview

**PlotterSqliteDataSource** provides persistent storage for Plotter entities (Projects, Folders, Notes) using SQLite with:

- ✅ **Proper Relational Design** - Foreign keys, not JSON blobs
- ✅ **Clean Architecture** - Separate layers for DTOs, Mappers, and DataSources
- ✅ **ACID Transactions** - SQLite's reliability
- ✅ **CASCADE Deletes** - Referential integrity maintained automatically
- ✅ **Health Monitoring** - Built-in metrics and health checks
- ✅ **Type Safety** - RAII wrappers for SQLite resources
- ✅ **Comprehensive Tests** - Full test coverage for all datasource operations

## Architecture

This library is part of a clean architecture layered system:

```
┌─────────────────────────────────────────────────────┐
│          Domain Entities (PlotterEntities)          │
│         Project, Folder, Note (business logic)      │
└─────────────────────────────────────────────────────┘
                          ▲
                          │ Mappers convert
                          │
┌─────────────────────────────────────────────────────┐
│        Mapper Layer (PlotterSqliteMappers)          │
│    SqliteProjectMapper, SqliteFolderMapper, etc.    │
│      Implements EntityDTOMapper interfaces          │
└─────────────────────────────────────────────────────┘
                          │
                    ┌─────┴─────┐
                    ▼           ▼
┌──────────────────────┐  ┌──────────────────────┐
│  Base DTOs           │  │ SQLite DTOs          │
│ (PlotterDTOs)        │  │(PlotterSqliteDTOs)   │
│ Marker interfaces    │  │ Concrete structures  │
└──────────────────────┘  └──────────────────────┘
                                  ▲
                                  │ Datasources persist
                                  │
┌─────────────────────────────────────────────────────┐
│     SQLite DataSources (PlotterSqliteDataSource)    │
│  SqliteProjectDataSource, SqliteFolderDataSource    │
│         Implements DataSource interfaces            │
└─────────────────────────────────────────────────────┘
                          ▲
                          │
┌─────────────────────────────────────────────────────┐
│              SQLite Database                        │
│  ┌───────────┐  ┌───────────┐  ┌────────────┐      │
│  │ projects  │──┤  folders  │──┤   notes    │      │
│  └───────────┘  └───────────┘  └────────────┘      │
│       Proper foreign keys & CASCADE constraints     │
└─────────────────────────────────────────────────────┘
```

### Layer Responsibilities

- **PlotterEntities**: Pure domain models with business logic
- **PlotterSqliteMappers**: Converts between entities and SQLite DTOs
- **PlotterSqliteDTOs**: SQLite-specific DTO structures (inherits from base DTOs)
- **PlotterSqliteDataSource**: Database operations (CRUD, queries, transactions)
- **PlotterRepositories**: Abstract interfaces for datasources and mappers

## Database Schema

### Relational Design (NOT JSON!)

```sql
-- Projects table
CREATE TABLE projects (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT,
    created_at INTEGER NOT NULL,
    updated_at INTEGER NOT NULL
);

-- Folders table with proper foreign keys
CREATE TABLE folders (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT,
    parent_project_id TEXT,      -- FK to projects.id
    parent_folder_id TEXT,        -- FK to folders.id (self-reference)
    created_at INTEGER NOT NULL,
    updated_at INTEGER NOT NULL,
    FOREIGN KEY (parent_project_id) REFERENCES projects(id) ON DELETE CASCADE,
    FOREIGN KEY (parent_folder_id) REFERENCES folders(id) ON DELETE CASCADE,
    CHECK (
        (parent_project_id IS NOT NULL AND parent_folder_id IS NULL) OR
        (parent_project_id IS NULL AND parent_folder_id IS NOT NULL) OR
        (parent_project_id IS NULL AND parent_folder_id IS NULL)
    )
);

-- Notes table
CREATE TABLE notes (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    path TEXT NOT NULL,
    content TEXT,                 -- Note content stored directly
    parent_folder_id TEXT,        -- FK to folders.id
    created_at INTEGER NOT NULL,
    updated_at INTEGER NOT NULL,
    FOREIGN KEY (parent_folder_id) REFERENCES folders(id) ON DELETE CASCADE
);

-- Junction tables for many-to-many relationships
CREATE TABLE folder_notes (
    folder_id TEXT NOT NULL,
    note_id TEXT NOT NULL,
    PRIMARY KEY (folder_id, note_id),
    FOREIGN KEY (folder_id) REFERENCES folders(id) ON DELETE CASCADE,
    FOREIGN KEY (note_id) REFERENCES notes(id) ON DELETE CASCADE
);

CREATE TABLE folder_subfolders (
    parent_folder_id TEXT NOT NULL,
    subfolder_id TEXT NOT NULL,
    PRIMARY KEY (parent_folder_id, subfolder_id),
    FOREIGN KEY (parent_folder_id) REFERENCES folders(id) ON DELETE CASCADE,
    FOREIGN KEY (subfolder_id) REFERENCES folders(id) ON DELETE CASCADE
);

CREATE TABLE project_folders (
    project_id TEXT NOT NULL,
    folder_id TEXT NOT NULL,
    PRIMARY KEY (project_id, folder_id),
    FOREIGN KEY (project_id) REFERENCES projects(id) ON DELETE CASCADE,
    FOREIGN KEY (folder_id) REFERENCES folders(id) ON DELETE CASCADE
);
```

**Why This Design?**

- **Foreign Keys** maintain referential integrity
- **CASCADE** deletes automatically clean up orphaned records
- **Indexes** on foreign keys for fast lookups
- **Normalized** - no duplicate data, easy to update
- **Junction Tables** - properly model many-to-many relationships
- **Relational Queries** - proper JOINs, not JSON parsing

## Key Components

### 1. SQLite DTOs (PlotterSqliteDTOs Library)

Simple structs representing database rows, defined in the separate `PlotterSqliteDTOs` library:

```cpp
struct SqliteProjectDTO : public dto::ProjectDTO {
    std::string id;
    std::string name;
    std::string description;
    long long createdAt;
    long long updatedAt;
    std::vector<std::string> folderIds;
};
```

**Purpose**: Keep database representation separate from business entities.

### 2. Mappers (PlotterSqliteMappers Library)

Convert between domain entities and DTOs, defined in the separate `PlotterSqliteMappers` library:

```cpp
class SqliteProjectMapper : public ProjectDTOMapper {
    dto::ProjectDTO* toDTO(const Project& entity) override;
    Project toEntity(const dto::ProjectDTO& dto) override;
};
```

**Purpose**: Handle conversions and bridge the gap between entities and database-specific DTOs.

### 3. DataSources (This Library)

Implement the `ProjectDataSource`, `FolderDataSource`, `NoteDataSource` interfaces:

```cpp
class SqliteProjectDataSource : public ProjectDataSource {
    std::string save(const dto::ProjectDTO& dto) override;
    std::optional<dto::ProjectDTO*> findById(const std::string& id) override;
    std::vector<dto::ProjectDTO*> findAll() override;
    bool deleteById(const std::string& id) override;
    // ... health checks, metrics, etc.
};
```

**Purpose**: Provide CRUD operations with proper relational queries, working with DTOs (not entities directly).

## Building

### Prerequisites

- C++17 or later
- CMake 3.16+
- SQLite3 development libraries
- PlotterEntities library
- PlotterDTOs library (base marker interfaces)
- PlotterSqliteDTOs library (concrete DTO structures)
- PlotterSqliteMappers library (entity ↔ DTO converters)
- PlotterRepositories library (datasource interfaces)

### Install SQLite3

**macOS:**
```bash
brew install sqlite3
```

**Linux:**
```bash
sudo apt-get install libsqlite3-dev
```

### Build Steps

```bash
cd PlotterSqliteDataSource
mkdir build && cd build
cmake ..
cmake --build .
```

### Build Options

```bash
# Build with tests
cmake .. -DBUILD_TESTS=ON
make
./tests/test_datasource

# Build with examples
cmake .. -DBUILD_EXAMPLES=ON
make
./examples/sqlite_demo
```

## Usage Example

```cpp
#include "plotter_sqlite/SqliteProjectDataSource.h"
#include "plotter_sqlite/SqliteFolderDataSource.h"
#include "plotter_sqlite_mappers/SqliteMappers.h"
#include "Project.h"
#include "Folder.h"

int main() {
    // Create datasources
    SqliteProjectDataSource projectDS("my-db", "./app.db");
    SqliteFolderDataSource folderDS("my-db", "./app.db");
    
    // Create mappers
    SqliteProjectMapper projectMapper;
    SqliteFolderMapper folderMapper;
    
    // Connect datasources
    projectDS.connect();
    folderDS.connect();
    
    // Create project entity and convert to DTO
    Project research("proj-1", "AI Research");
    research.setDescription("Machine learning project");
    auto projectDTO = projectMapper.toDTO(research);
    
    // Save via datasource (uses DTOs)
    projectDS.save(*projectDTO);
    delete projectDTO;
    
    // Create folder entity and convert to DTO
    Folder papers("folder-1", "Papers");
    papers.setDescription("Research papers");
    papers.setParentProjectId("proj-1");
    auto folderDTO = folderMapper.toDTO(papers);
    
    // Save folder
    folderDS.save(*folderDTO);
    delete folderDTO;
    
    // Query with relational integrity
    auto projectDtoOpt = projectDS.findById("proj-1");
    if (projectDtoOpt) {
        // Convert DTO back to entity
        Project project = projectMapper.toEntity(**projectDtoOpt);
        auto folderIds = project.getFolderIds();
        std::cout << "Project has " << folderIds.size() << " folders\n";
        delete *projectDtoOpt;
    }
    
    // CASCADE delete - removes all child folders and notes automatically!
    projectDS.deleteById("proj-1");
    
    return 0;
}
```

## Why DTOs and Mappers in Separate Libraries?

### The Problem: Circular Dependencies

If datasources, DTOs, and mappers were in one library:
- DataSource needs entities (to provide `save(Project&)`)
- Entities shouldn't know about database specifics
- Result: tight coupling ❌

### The Solution: Three Separate Libraries

✅ **PlotterSqliteDTOs** - Pure data structures
```cpp
struct SqliteProjectDTO : public dto::ProjectDTO {
    std::string id;
    std::string name;
    // Just data, no logic
};
```

✅ **PlotterSqliteMappers** - Conversion logic
```cpp
class SqliteProjectMapper : public ProjectDTOMapper {
    dto::ProjectDTO* toDTO(const Project& entity) override;
    Project toEntity(const dto::ProjectDTO& dto) override;
};
```

✅ **PlotterSqliteDataSource** - Database operations
```cpp
class SqliteProjectDataSource : public ProjectDataSource {
    std::string save(const dto::ProjectDTO& dto) override {
        // Works with DTOs, not entities
    }
};
```

**Benefits:**
1. **Clean separation** - Each library has one responsibility
2. **No circular dependencies** - Clear dependency direction
3. **Entities stay pure** - No database knowledge
4. **Easy to swap databases** - Create PlotterPostgresDTOs + PlotterPostgresMappers + PlotterPostgresDataSource
5. **Testable** - Can mock each layer independently

## Relational Queries vs. JSON

### ❌ What We DIDN'T Do (JSON in columns)

```sql
-- BAD: JSON array in column
CREATE TABLE projects (
    id TEXT PRIMARY KEY,
    folder_ids TEXT  -- "[\"folder-1\", \"folder-2\"]" 🤮
);
```

**Problems:**
- Can't enforce foreign keys
- No CASCADE deletes
- Slow queries (must parse JSON)
- Can't index efficiently
- Duplicate data

### ✅ What We DID Do (Proper foreign keys)

```sql
-- GOOD: Foreign key relationships
CREATE TABLE folders (
    id TEXT PRIMARY KEY,
    parent_project_id TEXT,
    FOREIGN KEY (parent_project_id) REFERENCES projects(id) ON DELETE CASCADE
);
```

**Benefits:**
- Database enforces referential integrity
- CASCADE deletes work automatically
- Fast indexed lookups
- Normalized data
- Standard SQL queries

## Testing

This library has comprehensive test coverage:

```bash
cd build
cmake .. -DBUILD_TESTS=ON
make
./tests/test_datasource
```

**Test Coverage:**
- ✅ Project CRUD operations
- ✅ Folder CRUD with foreign key constraints
- ✅ Note CRUD with parent relationships
- ✅ CASCADE delete behavior
- ✅ Health checks and metrics
- ✅ Database initialization

See `tests/test_datasource.cpp` for full test suite.

## Integration with PlotterRepositories

This datasource implements the interfaces from PlotterRepositories and works with the MultiSource repository pattern:

```cpp
#include "plotter_repositories/MultiSourceProjectRepository.h"
#include "plotter_repositories/DataSourceRouter.h"
#include "plotter_sqlite/SqliteProjectDataSource.h"
#include "plotter_sqlite_mappers/SqliteProjectMapper.h"

// Create datasource and mapper
auto sqliteDS = std::make_unique<SqliteProjectDataSource>("sqlite", "./db.sqlite");
auto mapper = std::make_unique<SqliteProjectMapper>();

// Create router
auto router = std::make_unique<DataSourceRouter<ProjectDataSource>>();
router->addDataSource(sqliteDS.get());

// Create repository with injected dependencies
auto repository = std::make_unique<MultiSourceProjectRepository<DataSourceRouter<ProjectDataSource>>>(
    router.get(),
    mapper.get()
);

// Now use with use cases
CreateProjectUseCase createProject(repository.get());
auto response = createProject.execute({"My Project", "Description"});
```

**Key Points:**
- Datasources work with **DTOs** (not entities)
- Mappers convert **entities ↔ DTOs**
- Repositories combine datasources + mappers to provide entity-based API for use cases

## Health Monitoring

All datasources support health checks:

```cpp
auto health = projectDS.checkHealth();
std::cout << "Status: " << (health.isHealthy() ? "HEALTHY" : "UNHEALTHY") << "\n";
std::cout << "Message: " << health.message << "\n";

auto metrics = projectDS.getMetrics();
std::cout << "Total requests: " << metrics.totalRequests << "\n";
std::cout << "Success rate: " << metrics.getSuccessRate() << "%\n";
std::cout << "Avg response time: " << metrics.averageResponseTimeMs << "ms\n";
```

## File Structure

```
PlotterSqliteDataSource/
├── CMakeLists.txt
├── README.md
├── include/
│   └── plotter_sqlite/
│       ├── PlotterSqlite.h            # Main header (includes all)
│       ├── SqliteDatabase.h           # RAII SQLite wrapper
│       ├── SqliteProjectDataSource.h  # Project datasource
│       ├── SqliteFolderDataSource.h   # Folder datasource
│       └── SqliteNoteDataSource.h     # Note datasource
├── src/
│   ├── SqliteDatabase.cpp             # Database + schema init
│   ├── SqliteProjectDataSource.cpp    # CRUD with relational queries
│   ├── SqliteFolderDataSource.cpp     # Folder operations
│   └── SqliteNoteDataSource.cpp       # Note operations
├── tests/
│   ├── test_datasource.cpp            # Comprehensive test suite
│   └── CMakeLists.txt
└── examples/
    ├── sqlite_demo.cpp                # Full working example
    └── CMakeLists.txt
```

## Dependencies

- **PlotterEntities** - Domain entities (Project, Folder, Note)
- **PlotterDTOs** - Base DTO marker interfaces
- **PlotterSqliteDTOs** - SQLite-specific DTO structures
- **PlotterRepositories** - DataSource interface definitions
- **SQLite3** - Database engine

## Related Libraries

- **PlotterEntities** - Domain entity definitions
- **PlotterDTOs** - Base DTO marker interfaces
- **PlotterSqliteDTOs** - SQLite DTO structures
- **PlotterSqliteMappers** - Entity ↔ DTO conversion logic
- **PlotterRepositories** - Repository interfaces and MultiSource implementations
- **PlotterUseCases** - Business logic layer
