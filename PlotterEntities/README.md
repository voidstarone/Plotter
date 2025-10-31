# PlotterEntities

A C++ domain model library for organizing notes with a hierarchical structure: Projects contain Folders, and Folders contain Notes.

## Purpose

This library defines the **pure domain entities** for the Plotter application following Clean Architecture principles. It contains:
- **Zero business logic dependencies** - no databases, no frameworks, no external libraries
- **Core domain models** - Project, Folder, Note entities
- **Entity relationships** - hierarchical structure with IDs

## Architecture Position

```
┌─────────────────────────────────────┐
│   Domain Layer (THIS LIBRARY)      │  ← Innermost layer
│   - Project                         │
│   - Folder                          │
│   - Note                            │
│   - FileItem (base class)           │
└─────────────────────────────────────┘
              ↑
              │ depends on
┌─────────────────────────────────────┐
│   Application Layer                 │
│   (PlotterUseCases)                 │
│   - Use cases depend on entities    │
└─────────────────────────────────────┘
              ↑
              │ depends on
┌─────────────────────────────────────┐
│   Infrastructure Layer              │
│   - Repositories, DataSources       │
│   - Map entities ↔ DTOs             │
└─────────────────────────────────────┘
```

## Project Structure

```
PlotterEntities/
├── CMakeLists.txt          # Build configuration
├── LIBRARY_README.md       # Library usage guide
├── include/               # Header files
│   ├── FileItem.h         # Base class for all entities
│   ├── Project.h          # Project entity
│   ├── Folder.h           # Folder entity
│   └── Note.h             # Note entity
├── src/                   # Source files
│   ├── FileItem.cpp
│   ├── Project.cpp
│   ├── Folder.cpp
│   └── Note.cpp
├── tests/                 # Unit tests
│   └── test_entities.cpp  # Entity behavior tests
└── examples/              # Usage examples
    └── library_usage.cpp  # Demo application
```

## Entities

### Project
- Contains multiple folders
- Has name, description, and unique ID
- Provides folder management (add, remove, find)
- Automatically generates timestamps

### Folder  
- Belongs to a Project or another Folder (supports nesting)
- Contains multiple notes and subfolders
- Has name, description, and unique ID
- Provides note management (add, remove, find)
- Provides subfolder management (add, remove, find)
- Supports unlimited nesting depth

### Note
- Belongs to a Folder
- Has title, content, and timestamps (created/updated)
- Supports arbitrary attributes via a dictionary (`std::unordered_map<std::string, std::any>`)
- Template-based attribute system supports any data type
- Automatically updates timestamps when modified

## Key Features

### Hierarchical Organization
- **Projects** → **Folders** → **Subfolders** → **Notes**
- Unlimited nesting depth for folders
- Each level can contain multiple items of the next level
- Unique IDs for all entities
- Tree visualization similar to Unix `tree` command

### Flexible Attributes
Notes support arbitrary key-value attributes:
```cpp
note->setAttribute<std::string>("author", "John Doe");
note->setAttribute<int>("priority", 5);
note->setAttribute<bool>("completed", true);
note->setAttribute<double>("rating", 4.5);
```

### Search and Navigation
- Find by ID or name at each level (including nested folders)
- Recursive note counting (includes notes in all subfolders)
- Count items and get collections
- Clear and bulk operations

### Timestamps
- Automatic creation and modification timestamps
- Human-readable timestamp formatting
- Updates automatically when content changes

## Building

```bash
cd PlotterEntities
mkdir build && cd build
cmake ..
make
```

### Build Options

```bash
# Build with tests
cmake .. -DBUILD_TESTS=ON
make
./tests/test_entities

# Build with examples
cmake .. -DBUILD_EXAMPLES=ON
make
./bin/library_usage
```

## Using as a Library

```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/PlotterEntities)
target_link_libraries(YourTarget PRIVATE PlotterEntities)
```

## Example Usage

```cpp
// Create a project
auto project = std::make_shared<Project>("Research Project");

// Add a top-level folder
auto folder = project->addFolder("Literature Review");

// Create nested folder structure
auto csFolder = folder->addSubfolder("Computer Science");
auto aiFolder = csFolder->addSubfolder("AI Research");

// Add a note to nested folder
auto note = aiFolder->addNote("Important Paper", "This paper discusses...");

// Add custom attributes
note->setAttribute<std::string>("author", "Smith et al.");
note->setAttribute<int>("year", 2023);
note->setAttribute<double>("impact_factor", 4.2);

// Retrieve attributes
std::string author = note->getAttribute<std::string>("author");
int year = note->getAttribute<int>("year", 2000); // with default value

// Display tree structure
std::cout << project->toTreeString() << std::endl;
```

### Tree Output Example
```
📁 Research Project
└── Literature Review (0 notes, 1 folders)
    └── Computer Science (0 notes, 1 folders)
        └── AI Research (1 notes)
            └── 📄 Important Paper
```

## Dependencies

- C++17 compatible compiler
- CMake 3.16 or higher
- Standard library (no external dependencies)

## Design Principles

1. **Memory Management**: Uses smart pointers (`std::shared_ptr`) for automatic memory management
2. **Type Safety**: Template-based attribute system with type checking
3. **Extensibility**: Easy to add new entity types or extend existing ones
4. **Performance**: Efficient search and storage using STL containers
5. **Maintainability**: Clear separation of concerns with header/source file organization