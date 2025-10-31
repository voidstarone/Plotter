# Clean Architecture Fix: Repository SQLite Hardcoding

## Problem
The repository implementations (`MultiSourceNoteRepository`, `MultiSourceProjectRepository`, `MultiSourceFolderRepository`) were hardcoded to use SQLite-specific DTOs and mappers. This violated clean architecture principles by creating tight coupling between the repository layer and a specific database implementation.

**Specific violations:**
1. Repositories directly included `SqliteEntityMappers.h` and `SqliteDTOs.h`
2. Repositories used `SqliteNoteMapper::toDTO()` and `SqliteNoteMapper::toEntity()` static methods
3. Repositories performed `dynamic_cast<SqliteNoteDTO*>` to check DTO types
4. Adding a new database (Postgres, MongoDB, etc.) would require modifying repository code

## Solution
Introduced abstract mapper interfaces following the Dependency Inversion Principle:

### 1. Created Abstract Mapper Interfaces
File: `PlotterRepositories/include/plotter_repositories/EntityDTOMapper.h`

```cpp
class ProjectDTOMapper {
public:
    virtual ~ProjectDTOMapper() = default;
    virtual dto::ProjectDTO* toDTO(const Project& entity) = 0;
    virtual Project toEntity(const dto::ProjectDTO& dto) = 0;
};

class FolderDTOMapper {
public:
    virtual ~FolderDTOMapper() = default;
    virtual dto::FolderDTO* toDTO(const Folder& entity) = 0;
    virtual Folder toEntity(const dto::FolderDTO& dto) = 0;
};

class NoteDTOMapper {
public:
    virtual ~NoteDTOMapper() = default;
    virtual dto::NoteDTO* toDTO(const Note& entity) = 0;
    virtual Note toEntity(const dto::NoteDTO& dto) = 0;
};
```

### 2. Updated Repository Constructors
Repositories now accept mapper instances via dependency injection:

**Before:**
```cpp
template<typename RouterType>
class MultiSourceNoteRepository : public NoteRepository {
public:
    explicit MultiSourceNoteRepository(RouterType* router);
private:
    RouterType* router;
};
```

**After:**
```cpp
template<typename RouterType>
class MultiSourceNoteRepository : public NoteRepository {
public:
    explicit MultiSourceNoteRepository(RouterType* router, NoteDTOMapper* dtoMapper);
private:
    RouterType* router;
    NoteDTOMapper* mapper;  // Abstract interface
};
```

### 3. Updated Repository Implementations
Changed all DTO conversion code to use the injected abstract mapper:

**Before (SQLite-specific):**
```cpp
std::string save(const Note& note) override {
    auto results = router->template executeWrite<std::string>(
        [&note](NoteDataSource* ds) {
            return ds->save(SqliteNoteMapper::toDTO(note));  // Hardcoded!
        }
    );
    // ...
}
```

**After (Database-agnostic):**
```cpp
std::string save(const Note& note) override {
    std::unique_ptr<dto::NoteDTO> noteDTO(mapper->toDTO(note));  // Uses injected mapper
    
    auto results = router->template executeWrite<std::string>(
        [&noteDTO](NoteDataSource* ds) {
            return ds->save(*noteDTO);
        }
    );
    // ...
}
```

### 4. Created Concrete Mapper Implementations
File: `PlotterSqliteDataSource/include/plotter_sqlite/SqliteEntityDTOMappers.h`

```cpp
class SqliteProjectDTOMapper : public repositories::ProjectDTOMapper {
public:
    dto::ProjectDTO* toDTO(const Project& entity) override;
    Project toEntity(const dto::ProjectDTO& dto) override;
};

class SqliteFolderDTOMapper : public repositories::FolderDTOMapper {
public:
    dto::FolderDTO* toDTO(const Folder& entity) override;
    Folder toEntity(const dto::FolderDTO& dto) override;
};

class SqliteNoteDTOMapper : public repositories::NoteDTOMapper {
public:
    dto::NoteDTO* toDTO(const Note& entity) override;
    Note toEntity(const dto::NoteDTO& dto) override;
};
```

### 5. Enhanced DTOs with Child Collections
Updated SQLite DTOs to include child entity IDs, allowing the datasource to populate complete aggregate data:

```cpp
struct SqliteProjectDTO : public plotter::dto::ProjectDTO {
    std::string id;
    std::string name;
    std::string description;
    long long createdAt;
    long long updatedAt;
    std::vector<std::string> folderIds;  // Added: populated by datasource
};

struct SqliteFolderDTO : public plotter::dto::FolderDTO {
    // ... existing fields ...
    std::vector<std::string> noteIds;       // Added
    std::vector<std::string> subfolderIds;  // Added
};
```

## Architecture Benefits

### Dependency Inversion Principle (DIP)
- High-level repositories now depend on abstract `EntityDTOMapper` interfaces
- Low-level datasources (SQLite) provide concrete mapper implementations
- Dependency arrows point inward: DataSource → Repository Interfaces

### Open/Closed Principle (OCP)
- Adding a new database (Postgres, MongoDB) requires:
  1. Create new DTO types (`PostgresProjectDTO`, etc.)
  2. Implement mapper interfaces (`PostgresProjectDTOMapper`, etc.)
  3. Implement datasource interfaces (`PostgresProjectDataSource`, etc.)
- **NO changes to repository code required**

### Clean Architecture Layers Respected
```
┌─────────────────────────────────────────┐
│         Entities (Pure Domain)          │  ← Innermost
│   Project, Folder, Note (no infra)      │
└─────────────────────────────────────────┘
              ↑
┌─────────────────────────────────────────┐
│         Use Cases / Repositories        │
│  Repository Interfaces (abstract)       │
│  EntityDTOMapper Interfaces (abstract)  │
└─────────────────────────────────────────┘
              ↑
┌─────────────────────────────────────────┐
│     Repository Implementations          │
│  MultiSourceNoteRepository (template)   │
│  Uses injected NoteDTOMapper*           │
└─────────────────────────────────────────┘
              ↑
┌─────────────────────────────────────────┐
│    DataSource Implementations           │
│  SqliteNoteDataSource                   │
│  SqliteNoteDTOMapper (concrete)         │
│  SqliteNoteDTO (concrete)               │
└─────────────────────────────────────────┘  ← Outermost
```

### Database Abstraction
- Repositories work with base `dto::NoteDTO*` pointers (opaque)
- Repositories call `mapper->toDTO(entity)` and `mapper->toEntity(dto)`
- Mappers handle concrete DTO type conversion internally using `dynamic_cast`
- DataSources are responsible for populating DTOs with all necessary data (including child IDs)

## Files Modified

### PlotterRepositories
- `include/plotter_repositories/EntityDTOMapper.h` (NEW) - Abstract mapper interfaces
- `include/plotter_repositories/MultiSourceNoteRepository.h` - Updated to use abstract mapper
- `include/plotter_repositories/MultiSourceProjectRepository.h` - Updated to use abstract mapper
- `include/plotter_repositories/MultiSourceFolderRepository.h` - Updated to use abstract mapper

### PlotterSqliteDataSource
- `include/plotter_sqlite/SqliteEntityDTOMappers.h` (NEW) - Concrete SQLite mappers
- `src/SqliteEntityDTOMappers.cpp` (NEW) - Mapper implementations
- `include/plotter_sqlite/SqliteDTOs.h` - Added folderIds, noteIds, subfolderIds fields
- `CMakeLists.txt` - Added SqliteEntityDTOMappers.cpp to build, added PlotterEntities dependency

## Usage Example

```cpp
// Create datasource
SqliteDatabase db("my_database.db");
SqliteNoteDataSource noteDataSource(&db);

// Create router
SimpleDataSourceRouter<NoteDataSource> router;
router.addDataSource(&noteDataSource);

// Create concrete mapper for SQLite
SqliteNoteDTOMapper sqliteMapper;

// Create repository with injected dependencies
MultiSourceNoteRepository<decltype(router)> noteRepo(&router, &sqliteMapper);

// Use repository (database-agnostic!)
Note note("note-1", "My Note", "/notes/note1.txt", "folder-1");
note.setContent("Hello, World!");
std::string savedId = noteRepo.save(note);

auto retrievedNote = noteRepo.findById(savedId);
```

## Build Verification

```bash
cd PlotterRepositories/build
cmake -DBUILD_EXAMPLES=OFF .. && make PlotterRepositories
# ✅ Build successful

cd PlotterSqliteDataSource/build
make PlotterSqliteDataSource
# ✅ Build successful
```

## Next Steps

1. **Update instantiation code**: All code that creates repositories must now provide mapper instances
2. **Update examples**: Fix demo programs to pass mappers to repository constructors
3. **Create Postgres/MongoDB implementations**: Follow same pattern with new DTO types and mappers
4. **Add tests**: Verify mappers correctly convert between entities and DTOs
5. **Document mapper contract**: Clarify that DTOs must include all child IDs for aggregate roots

## Architectural Principles Honored

✅ **Single Responsibility**: Each mapper handles one entity type's conversion  
✅ **Open/Closed**: New databases don't require repository changes  
✅ **Liskov Substitution**: Any `NoteDTOMapper` implementation works with repositories  
✅ **Interface Segregation**: Separate mapper interfaces for each entity type  
✅ **Dependency Inversion**: Repositories depend on abstractions, not concrete mappers  
✅ **Clean Architecture**: Dependencies point inward; outer layers can be swapped  
