# PlotterRepositories

A C++ template-based repository library providing database-agnostic data access following Clean Architecture principles.

## Overview

**PlotterRepositories** provides:
- **Repository interfaces** that use cases depend on (ProjectRepository, FolderRepository, NoteRepository)
- **EntityDTOMapper interfaces** for converting entities ↔ DTOs
- **MultiSource repository implementations** using templates for type safety
- **DataSource interfaces** that concrete implementations must implement

This is a **header-only template library** with concrete mapper implementations.

## Architecture

```
┌────────────────────────────────────────────────┐
│         Use Cases Layer                        │
│   (PlotterUseCases)                            │
│   - Depends on repository interfaces           │
└────────────────────────────────────────────────┘
                    ↓ depends on
┌────────────────────────────────────────────────┐
│   Repository Interfaces (THIS LIBRARY)         │
│   ┌──────────────────────────────────────┐    │
│   │ Repository Interfaces:               │    │
│   │  - ProjectRepository                 │    │
│   │  - FolderRepository                  │    │
│   │  - NoteRepository                    │    │
│   │                                      │    │
│   │ EntityDTOMapper Interfaces:          │    │
│   │  - ProjectDTOMapper                  │    │
│   │  - FolderDTOMapper                   │    │
│   │  - NoteDTOMapper                     │    │
│   │                                      │    │
│   │ Template Implementations:            │    │
│   │  - MultiSourceProjectRepository<R>   │    │
│   │  - MultiSourceFolderRepository<R>    │    │
│   │  - MultiSourceNoteRepository<R>      │    │
│   │                                      │    │
│   │ DataSource Interfaces:               │    │
│   │  - DataSource (base)                 │    │
│   │  - ProjectDataSource                 │    │
│   │  - FolderDataSource                  │    │
│   │  - NoteDataSource                    │    │
│   └──────────────────────────────────────┘    │
└────────────────────────────────────────────────┘
                    ↓ implements
┌────────────────────────────────────────────────┐
│   Concrete Implementations (External)          │
│   - SqliteProjectDataSource                    │
│   - SqliteProjectMapper                        │
│   - (Future: Postgres, Redis, etc.)            │
└────────────────────────────────────────────────┘
```

## Key Components

### 1. Repository Interfaces

Abstract interfaces that use cases depend on:

```cpp
class ProjectRepository {
    virtual std::string save(const Project& project) = 0;
    virtual std::optional<Project> findById(const std::string& id) = 0;
    virtual std::vector<Project> findAll() = 0;
    virtual bool update(const Project& project) = 0;
    virtual bool deleteById(const std::string& id) = 0;
    virtual size_t count() = 0;
    virtual void clear() = 0;
};

// Similar interfaces for FolderRepository and NoteRepository
```

### 2. EntityDTOMapper Interfaces

Abstract mappers for converting entities ↔ DTOs:

```cpp
class ProjectDTOMapper {
    virtual dto::ProjectDTO* toDTO(const Project& entity) = 0;
    virtual Project toEntity(const dto::ProjectDTO& dto) = 0;
};

// Similar interfaces for FolderDTOMapper and NoteDTOMapper
```

**Why separate mappers?**
- Repositories remain database-agnostic (no knowledge of SQLite, Postgres, etc.)
- Different databases can use different DTO structures
- Dependency inversion: repositories depend on mapper interface, not concrete implementation

### 3. DataSource Interfaces

Base interface for all data sources:

```cpp
class DataSource {
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0;
    virtual bool isAvailable() const = 0;
    virtual HealthCheckResult checkHealth() = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
};
```

Entity-specific interfaces extending the base:

```cpp
class ProjectDataSource : public DataSource {
    virtual std::string save(const dto::ProjectDTO& dto) = 0;
    virtual std::optional<dto::ProjectDTO*> findById(const std::string& id) = 0;
    virtual std::vector<dto::ProjectDTO*> findAll() = 0;
    virtual bool update(const dto::ProjectDTO& dto) = 0;
    virtual bool deleteById(const std::string& id) = 0;
    virtual size_t count() = 0;
    virtual void clear() = 0;
};

// Similar for FolderDataSource, NoteDataSource
```

### 4. MultiSource Repository Templates

Template-based repository implementations:

```cpp
template<typename RouterImpl>
class MultiSourceProjectRepository : public ProjectRepository {
public:
    MultiSourceProjectRepository(RouterImpl* router, ProjectDTOMapper* mapper);
    
    std::string save(const Project& project) override {
        // Convert entity → DTO using mapper
        auto dto = mapper->toDTO(project);
        
        // Execute on router (writes to all datasources)
        auto results = router->executeWrite([&dto](ProjectDataSource* ds) {
            return ds->save(*dto);
        });
        
        return results[0]; // Return first successful ID
    }
    
    std::optional<Project> findById(const std::string& id) override {
        // Execute on router (reads from first available)
        return router->executeRead([&id, this](ProjectDataSource* ds) {
            auto dtoOpt = ds->findById(id);
            if (!dtoOpt) return std::optional<Project>{};
            
            // Convert DTO → entity using mapper
            return std::optional<Project>(mapper->toEntity(**dtoOpt));
        });
    }
    
    // ... other methods
};
```
## Usage Example

### 1. Implement DataSource

```cpp
class SqliteProjectDataSource : public ProjectDataSource {
    sqlite3* db;
    
public:
    std::string save(const dto::ProjectDTO& dto) override {
        // Downcast to concrete type
        auto& sqliteDto = dynamic_cast<const SqliteProjectDTO&>(dto);
        
        // Save to SQLite database
        // ... INSERT query using sqliteDto fields
        
        return sqliteDto.id;
    }
    
    std::optional<dto::ProjectDTO*> findById(const std::string& id) override {
        // Query database
        // ... SELECT query
        
        // Return concrete DTO (as base pointer)
        return new SqliteProjectDTO{...};
    }
    
    // Implement other methods...
};
```

### 2. Implement Mapper

```cpp
class SqliteProjectMapper : public ProjectDTOMapper {
public:
    dto::ProjectDTO* toDTO(const Project& entity) override {
        auto* dto = new SqliteProjectDTO();
        dto->id = entity.getId();
        dto->name = entity.getName();
        dto->description = entity.getDescription();
        dto->createdAt = entity.getCreatedAt();
        dto->updatedAt = entity.getUpdatedAt();
        dto->folderIds = entity.getFolderIds();
        return dto;
    }
    
    Project toEntity(const dto::ProjectDTO& baseDto) override {
        // Downcast to concrete type
        auto& dto = dynamic_cast<const SqliteProjectDTO&>(baseDto);
        
        Project project(dto.name, dto.id);
        project.setDescription(dto.description);
        // Set folder IDs, timestamps, etc.
        return project;
    }
};
```

### 3. Create Router and Repository

```cpp
// Create datasource and mapper
auto datasource = std::make_unique<SqliteProjectDataSource>("db.sqlite");
auto mapper = std::make_unique<SqliteProjectMapper>();

// Create router (manages multiple datasources)
auto router = std::make_unique<DataSourceRouter<ProjectDataSource>>();
router->addDataSource(datasource.get());

// Create repository
auto repository = std::make_unique<MultiSourceProjectRepository<DataSourceRouter<ProjectDataSource>>>(
    router.get(), mapper.get()
);

// Use with use cases
CreateProjectUseCase useCase(repository.get());
auto response = useCase.execute({"My Project", "Description"});
```

## Benefits

### Clean Architecture
- **Dependency Inversion**: Use cases depend on repository interfaces, not implementations
- **Database Independence**: Swap SQLite for Postgres without changing use case code
- **Separation of Concerns**: Entities, mappers, and datasources are separate

### Type Safety
- **Template-based**: Compile-time type checking for router types
- **Abstract interfaces**: Consistent API across all datasources

### Testability
- **Mock implementations**: Easy to create in-memory mocks for testing
- **Dependency injection**: Inject mock repositories into use cases

### Flexibility
- **Multi-source support**: Write to multiple databases simultaneously
- **Pluggable mappers**: Different databases can use different DTO structures

## Testing

Create mock implementations for testing:

```cpp
class MockProjectRepository : public ProjectRepository {
    std::map<std::string, Project> storage;
    
public:
    std::string save(const Project& project) override {
        storage[project.getId()] = project;
        return project.getId();
    }
    
    std::optional<Project> findById(const std::string& id) override {
        auto it = storage.find(id);
        return (it != storage.end()) ? std::optional{it->second} : std::nullopt;
    }
    
    // ... other methods
};
```

## Building

This is a header-only template library with some concrete implementations:

```bash
cd PlotterRepositories
mkdir build && cd build
cmake ..
make
```

### Build Options

```bash
# Build with tests
cmake .. -DBUILD_TESTS=ON
make
./tests/test_repository

# Build with examples
cmake .. -DBUILD_EXAMPLES=ON
make
./examples/simple_demo
```

## Project Structure

```
PlotterRepositories/
├── include/plotter_repositories/
│   ├── DataSource.h                      # Base datasource interface
│   ├── ProjectDataSource.h               # Project datasource interface
│   ├── FolderDataSource.h                # Folder datasource interface
│   ├── NoteDataSource.h                  # Note datasource interface
│   ├── DTOMapper.h                       # Base mapper interface
│   ├── EntityDTOMapper.h                 # Entity↔DTO mapper interfaces
│   ├── MultiSourceProjectRepository.h    # Template repository for projects
│   ├── MultiSourceFolderRepository.h     # Template repository for folders
│   ├── MultiSourceNoteRepository.h       # Template repository for notes
│   ├── DataSourceRouter.h                # Router template
│   └── PlotterRepositories.h             # Main header (includes all)
├── src/
│   ├── EntityMappers.cpp                 # Concrete mapper implementations
│   ├── MultiSourceProjectRepository.cpp  # Template instantiations
│   ├── MultiSourceFolderRepository.cpp
│   └── MultiSourceNoteRepository.cpp
├── tests/
│   ├── test_repository.cpp               # Repository tests
│   └── mocks/                            # Mock implementations for testing
│       ├── MockProjectDataSource.h
│       └── README.md
└── examples/
    ├── simple_demo.cpp                   # Basic usage example
    └── demo_multi_source.cpp             # Multi-datasource example
```

## Dependencies

- **PlotterEntities** - Domain entities (Project, Folder, Note)
- **PlotterDTOs** - Base DTO marker interfaces
- C++17 or later

## Related Libraries

- **PlotterEntities** - Domain entity definitions
- **PlotterDTOs** - Base DTO marker interfaces
- **PlotterSqliteDataSource** - SQLite datasource implementation
- **PlotterSqliteMappers** - SQLite mapper implementations
- **PlotterUseCases** - Use case layer that depends on repository interfaces
