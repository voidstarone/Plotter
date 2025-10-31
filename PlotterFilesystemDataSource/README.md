# PlotterFilesystemDataSource

A filesystem-based data source implementation for the Plotter project. This provides an alternative to SQLite storage by using actual files and directories with hidden dotfiles to track metadata.

## Overview

The FilesystemDataSource stores Plotter data directly in the filesystem:

- **Projects** → Directories with `.plotter_project` metadata file
- **Folders** → Subdirectories with `.plotter_folder` metadata file  
- **Notes** → Text/Markdown files with `.plotter_meta` companion file

## Example Structure

```
~/PlotterData/
├── MyProject/
│   ├── .plotter_project              # Project metadata (ID, description, timestamps)
│   ├── Research/
│   │   ├── .plotter_folder           # Folder metadata (ID, parent refs)
│   │   ├── article1.md
│   │   ├── article1.md.plotter_meta  # Note metadata (ID, timestamps)
│   │   ├── article2.md
│   │   ├── article2.md.plotter_meta
│   │   └── Papers/
│   │       ├── .plotter_folder
│   │       ├── paper1.md
│   │       └── paper1.md.plotter_meta
│   └── Ideas/
│       ├── .plotter_folder
│       ├── brainstorm.md
│       └── brainstorm.md.plotter_meta
└── AnotherProject/
    └── .plotter_project
```

## Features

### Natural Filesystem Organization
- Projects, folders, and notes are real directories and files
- Can be browsed and edited with any file manager or text editor
- Version control friendly (Git, etc.)

### Hidden Metadata
- All Plotter-specific metadata stored in hidden dotfiles
- Non-intrusive - doesn't pollute visible file listings
- JSON format for easy debugging and manual editing if needed

### UUID-based Identity
- Each entity has a unique UUID stored in its metadata file
- Allows entities to be moved/renamed while maintaining references
- IDs are stable across renames and moves

### Flexible File Extensions
- Configurable default extension (default: `.md`)
- Supports any text-based file format

## Metadata File Format

### .plotter_project
```json
{
   "id": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
   "name": "MyProject",
   "description": "My project description",
   "createdAt": 1698765432000,
   "updatedAt": 1698765432000,
   "folderIds": ["folder-id-1", "folder-id-2"]
}
```

### .plotter_folder
```json
{
   "id": "folder-uuid",
   "name": "Research",
   "description": "Research notes",
   "parentProjectId": "project-uuid",
   "parentFolderId": "",
   "createdAt": 1698765432000,
   "updatedAt": 1698765432000,
   "noteIds": ["note-id-1", "note-id-2"],
   "subfolderIds": ["subfolder-id-1"]
}
```

### .plotter_meta (for notes)
```json
{
   "id": "note-uuid",
   "name": "article1",
   "parentFolderId": "folder-uuid",
   "createdAt": 1698765432000,
   "updatedAt": 1698765432000
}
```

## Building

```bash
cd PlotterFilesystemDataSource
mkdir -p build && cd build
cmake ..
make
```

## Dependencies

- C++17 or later
- jsoncpp (for JSON metadata handling)
- PlotterDTOs (base DTO interfaces)
- PlotterFilesystemDTOs (filesystem-specific DTOs)

## Usage

```cpp
#include "plotter_filesystem/FilesystemDataSource.h"

using namespace plotter::filesystem;

// Create data sources
auto projectDS = std::make_unique<FilesystemProjectDataSource>(
    "filesystem-project", 
    "/path/to/plotter/data"
);
projectDS->connect();

auto folderDS = std::make_unique<FilesystemFolderDataSource>(
    "filesystem-folder",
    "/path/to/plotter/data"
);
folderDS->connect();

auto noteDS = std::make_unique<FilesystemNoteDataSource>(
    "filesystem-note",
    "/path/to/plotter/data",
    ".md"  // default extension
);
noteDS->connect();

// Use with repositories and routers...
```

## Advantages

- **Human-readable**: All data is in plain text files
- **Tool-agnostic**: Edit with any text editor
- **Backup-friendly**: Standard filesystem backup tools work
- **Version control**: Easy to use with Git/SVN/etc.
- **Transparent**: No database layer to debug

## Considerations

- Slower than SQLite for large datasets with complex queries
- ID lookups require filesystem scanning (cached in production use)
- Better suited for human-scale projects (hundreds/thousands of notes)
- Not recommended for high-frequency automated operations

## License

Part of the Plotter project.

