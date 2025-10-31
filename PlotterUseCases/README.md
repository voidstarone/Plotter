# PlotterUseCases - Clean Architecture Use Cases Library

This library implements the Application Layer (Use Cases) for the NoteTaker system, following Clean Architecture principles. It builds upon the entities defined in the PlotterEntities library.

## Architecture Overview

```
┌─────────────────────────────────────────┐
│   Presentation Layer (Future)           │
│   - CLI, Web API, GUI                   │
└─────────────────────────────────────────┘
               ↓ depends on
┌─────────────────────────────────────────┐
│   Application Layer (THIS LIBRARY)      │ ← Use Cases
│   - CreateProjectUseCase                │
│   - MoveNoteUseCase                     │
│   - etc.                                │
└─────────────────────────────────────────┘
               ↓ depends on
┌─────────────────────────────────────────┐
│   Repository Interfaces                 │
│   (PlotterRepositories)                 │
│   - ProjectRepository                   │
│   - FolderRepository                    │
│   - NoteRepository                      │
└─────────────────────────────────────────┘
               ↓ depends on
┌─────────────────────────────────────────┐
│   Domain Layer                          │
│   (PlotterEntities)                     │
│   - Project, Folder, Note               │
└─────────────────────────────────────────┘
               ↑ implemented by
┌─────────────────────────────────────────┐
│   Infrastructure Layer (External)       │
│   ┌─────────────────────────────────┐   │
│   │ PlotterSqliteDataSource         │   │
│   │ PlotterSqliteMappers            │   │
│   │ PlotterSqliteDTOs               │   │
│   └─────────────────────────────────┘   │
│   (Future: Postgres, Redis, etc.)       │
└─────────────────────────────────────────┘
```

## Project Structure

```
PlotterUseCases/
├── include/
│   ├── repositories/           # Repository interfaces
│   │   ├── ProjectRepository.h
│   │   ├── FolderRepository.h
│   │   └── NoteRepository.h
│   └── usecases/              # Use case headers
│       ├── CreateProjectUseCase.h
│       ├── GetProjectUseCase.h
│       ├── ListProjectsUseCase.h
│       ├── DeleteProjectUseCase.h
│       ├── CreateFolderUseCase.h
│       ├── MoveFolderUseCase.h
│       ├── CreateNoteUseCase.h
│       ├── MoveNoteUseCase.h
│       └── GetNoteContentUseCase.h
├── src/
│   └── usecases/              # Use case implementations
│       ├── CreateProjectUseCase.cpp
│       ├── GetProjectUseCase.cpp
│       └── ... (other use cases)
├── tests/
│   ├── test_usecases.cpp      # Use case tests
│   └── mocks/                 # Mock repository implementations (for testing)
│       ├── InMemoryProjectRepository.h
│       ├── InMemoryFolderRepository.h
│       └── InMemoryNoteRepository.h
├── examples/
│   ├── demo_application.cpp   # Complete demo showing usage
│   └── CMakeLists.txt
└── CMakeLists.txt
```

## Key Design Principles

### 1. Repository Interfaces
This library **defines repository interfaces** in `include/repositories/` that act as contracts between the application layer and infrastructure layer.

The mock implementations in `tests/mocks/` are **ONLY for testing**. In production, concrete implementations come from infrastructure libraries like `PlotterRepositories` with `PlotterSqliteDataSource`.

### 2. Use Case Pattern
Each use case follows a consistent pattern:
- **Request DTO**: Input parameters
- **Response DTO**: Output including success/error status
- **Validation**: Input validation with clear error messages
- **Single Responsibility**: Each use case does one thing well
- **Dependency Injection**: Repositories injected via constructor

### 3. Clean Error Handling
All use cases return structured responses with:
- `success` boolean flag
- `errorMessage` for user-friendly error descriptions
- Typed result data when successful

## Available Use Cases

### Project Management
- **CreateProjectUseCase**: Create a new project
- **GetProjectUseCase**: Retrieve project by ID
- **ListProjectsUseCase**: List all projects
- **DeleteProjectUseCase**: Delete project and all contents (cascading)

### Folder Management
- **CreateFolderUseCase**: Create folder in project or parent folder
- **MoveFolderUseCase**: Move folder to different parent

### Note Management
- **CreateNoteUseCase**: Create note in folder with optional initial content
- **GetNoteContentUseCase**: Retrieve note content (lazy loaded)
- **MoveNoteUseCase**: Move note between folders

## Usage Example

```cpp
#include <memory>
#include "usecases/CreateProjectUseCase.h"
#include "repositories/ProjectRepository.h"

// In real applications, repository comes from infrastructure layer
// e.g., MultiSourceProjectRepository from PlotterRepositories
auto projectRepo = createProductionRepository(); // from infrastructure

// Create use case with dependency injection
auto createProject = std::make_shared<CreateProjectUseCase>(projectRepo);

// Execute use case
CreateProjectUseCase::Request request;
request.name = "My Project";
request.description = "A sample project";

auto response = createProject->execute(request);

if (response.success) {
    std::cout << "Project created: " << response.projectId << std::endl;
} else {
    std::cerr << "Error: " << response.errorMessage << std::endl;
}
```

## Building

### Prerequisites
- C++17 or later
- CMake 3.16+
- PlotterEntities library (must be in `../PlotterEntities`)

### Build Commands

```bash
# Create build directory
mkdir build && cd build

# Configure (includes examples by default)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make

# Run demo
./bin/use_cases_demo
```

### Build Options

```bash
# Build without examples
cmake .. -DBUILD_EXAMPLES=OFF

# Build with tests
cmake .. -DBUILD_TESTS=ON
```

## Dependency Injection

In a real application, you would set up dependency injection like this:

```cpp
// Infrastructure layer setup
auto sqliteDataSource = std::make_shared<SqliteProjectDataSource>("app.db");
auto sqliteMapper = std::make_shared<SqliteProjectMapper>();
auto router = std::make_shared<DataSourceRouter<ProjectDataSource>>();
router->addDataSource(sqliteDataSource.get());

// Create repositories
auto projectRepo = std::make_shared<MultiSourceProjectRepository<...>>(
    router.get(), sqliteMapper.get()
);
auto folderRepo = std::make_shared<MultiSourceFolderRepository<...>>(...);
auto noteRepo = std::make_shared<MultiSourceNoteRepository<...>>(...);

// Application layer setup (use cases)
auto createProject = std::make_shared<CreateProjectUseCase>(projectRepo);
auto createFolder = std::make_shared<CreateFolderUseCase>(folderRepo, projectRepo);
auto createNote = std::make_shared<CreateNoteUseCase>(noteRepo, folderRepo);

// Use cases are now ready to be injected into presentation layer
// (CLI commands, web controllers, GUI event handlers, etc.)
```

## Extension Points

To add new use cases:

1. **Create header** in `include/usecases/YourUseCase.h`
2. **Follow the pattern**: Request/Response DTOs, validation, error handling
3. **Implement** in `src/usecases/YourUseCase.cpp`
4. **Add to CMakeLists.txt** in `USECASE_SOURCES`
5. **Write tests** using the mock repositories in `tests/mocks/`

## Dependencies

- **PlotterEntities**: Provides entity classes (Project, Folder, Note, etc.)
- **Standard Library**: Only uses STL, no external dependencies

## License

This library is designed to demonstrate Clean Architecture principles in C++. It's part of the PlotterEntities/PlotterUseCases educational project.