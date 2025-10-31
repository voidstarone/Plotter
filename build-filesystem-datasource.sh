#!/bin/bash

# Build script for Plotter Filesystem Data Source modules
# This script builds all three filesystem components in order

set -e  # Exit on error

echo "================================================"
echo "Building Plotter Filesystem Data Source Modules"
echo "================================================"
echo ""

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Get the script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Check for jsoncpp
echo -e "${BLUE}Checking dependencies...${NC}"
if ! pkg-config --exists jsoncpp; then
    echo -e "${RED}Error: jsoncpp not found!${NC}"
    echo "Please install jsoncpp:"
    echo "  macOS: brew install jsoncpp"
    echo "  Ubuntu/Debian: sudo apt-get install libjsoncpp-dev"
    exit 1
fi
echo -e "${GREEN}✓ jsoncpp found${NC}"
echo ""

# Function to build a module
build_module() {
    local module_name=$1
    local module_path=$2

    echo -e "${BLUE}Building ${module_name}...${NC}"

    cd "$module_path"

    # Create build directory if it doesn't exist
    mkdir -p build
    cd build

    # Clean previous build
    rm -rf *

    # Configure with CMake
    echo "  Configuring..."
    cmake .. > /dev/null 2>&1 || {
        echo -e "${RED}✗ CMake configuration failed for ${module_name}${NC}"
        return 1
    }

    # Build
    echo "  Compiling..."
    make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 2) > /dev/null 2>&1 || {
        echo -e "${RED}✗ Build failed for ${module_name}${NC}"
        return 1
    }

    echo -e "${GREEN}✓ ${module_name} built successfully${NC}"

    # Run tests
    echo "  Running tests..."
    if make test > /dev/null 2>&1; then
        echo -e "${GREEN}✓ Tests passed${NC}"
    else
        echo -e "${RED}✗ Tests failed${NC}"
        return 1
    fi

    cd "$SCRIPT_DIR"
    echo ""
}

# Build modules in order
echo "Starting build process..."
echo ""

build_module "PlotterFilesystemDTOs" "$SCRIPT_DIR/PlotterFilesystemDTOs"
build_module "PlotterFilesystemMappers" "$SCRIPT_DIR/PlotterFilesystemMappers"
build_module "PlotterFilesystemDataSource" "$SCRIPT_DIR/PlotterFilesystemDataSource"

echo "================================================"
echo -e "${GREEN}All modules built successfully!${NC}"
echo "================================================"
echo ""
echo "Next steps:"
echo "1. Review the FILESYSTEM_DATASOURCE_GUIDE.md for integration instructions"
echo "2. Consider adding filesystem backend option to your CLI"
echo "3. Test with real data by creating a project using the filesystem backend"
echo ""
echo "Example directory structure will be created at your chosen root path:"
echo "  YourProject/"
echo "    ├── .plotter_project"
echo "    ├── Folder1/"
echo "    │   ├── .plotter_folder"
echo "    │   ├── note1.md"
echo "    │   └── note1.md.plotter_meta"
echo "    └── Folder2/"
echo "        └── .plotter_folder"
echo ""
# Plotter Filesystem Data Source - Getting Started

This guide will help you build and integrate the new filesystem-based data source into your Plotter project.

## Overview

The filesystem data source provides an alternative to SQLite storage by using actual files and directories with hidden dotfiles to track metadata. This makes your Plotter data:

- **Human-readable**: All data stored in plain text files
- **Tool-friendly**: Edit with any text editor or file manager
- **Version control ready**: Works seamlessly with Git
- **Transparent**: No database layer - just files and folders

## Components Created

1. **PlotterFilesystemDTOs** - Data Transfer Objects for filesystem storage
2. **PlotterFilesystemMappers** - Mappers between entities and DTOs
3. **PlotterFilesystemDataSource** - The actual data source implementation

## Building Each Module

### 1. Build PlotterFilesystemDTOs

```bash
cd PlotterFilesystemDTOs/build
cmake ..
make
make test  # Run tests
```

### 2. Build PlotterFilesystemMappers

```bash
cd PlotterFilesystemMappers/build
cmake ..
make
make test  # Run tests
```

### 3. Build PlotterFilesystemDataSource

```bash
cd PlotterFilesystemDataSource/build
cmake ..
make
make test  # Run tests
```

## Dependencies

You'll need to install jsoncpp for JSON metadata handling:

### macOS (Homebrew)
```bash
brew install jsoncpp
```

### Ubuntu/Debian
```bash
sudo apt-get install libjsoncpp-dev
```

## Integration with Plotter CLI

To use the filesystem data source in your CLI application, you'll need to modify `PlotterObjCBridge.mm` to support filesystem data sources alongside SQLite.

### Example Integration Pattern

```cpp
// In PlotterObjCBridge.mm
#include "plotter_filesystem/FilesystemDataSource.h"
#include "plotter_filesystem_mappers/FilesystemMappers.h"

using namespace plotter::filesystem;
using namespace plotter::filesystem_mappers;

// Add to @implementation PLTPlotter
std::unique_ptr<FilesystemProjectDataSource> _fsProjectDS;
std::unique_ptr<FilesystemFolderDataSource> _fsFolderDS;
std::unique_ptr<FilesystemNoteDataSource> _fsNoteDS;

std::unique_ptr<FilesystemProjectMapper> _fsProjectMapper;
std::unique_ptr<FilesystemFolderMapper> _fsFolderMapper;
std::unique_ptr<FilesystemNoteMapper> _fsNoteMapper;

// Add initializer method for filesystem backend
- (nullable instancetype)initWithFilesystemPath:(NSString *)rootPath {
    if (self = [super init]) {
        const char *path = [rootPath UTF8String];

        try {
            // Create filesystem data sources
            _fsProjectDS = std::make_unique<FilesystemProjectDataSource>(
                "filesystem-project", path
            );
            _fsProjectDS->connect();

            _fsFolderDS = std::make_unique<FilesystemFolderDataSource>(
                "filesystem-folder", path
            );
            _fsFolderDS->connect();

            _fsNoteDS = std::make_unique<FilesystemNoteDataSource>(
                "filesystem-note", path, ".md"
            );
            _fsNoteDS->connect();

            // Create mappers
            _fsProjectMapper = std::make_unique<FilesystemProjectMapper>();
            _fsFolderMapper = std::make_unique<FilesystemFolderMapper>();
            _fsNoteMapper = std::make_unique<FilesystemNoteMapper>();

            // Create routers and repositories (similar to SQLite setup)
            // ...

        } catch (const std::exception& e) {
            NSLog(@"Failed to initialize filesystem data source: %s", e.what());
            return nil;
        }
    }
    return self;
}
```

## Usage Example

Once integrated, you can create a Plotter instance that uses the filesystem:

```swift
// In your Swift CLI code
let plotterPath = FileManager.default.homeDirectoryForCurrentUser
    .appendingPathComponent("PlotterData")
    .path

let plotter = PLTPlotter(filesystemPath: plotterPath)

// Now use plotter as normal - it will store data in ~/PlotterData/
```

## Directory Structure Example

After creating some projects, your filesystem will look like:

```
~/PlotterData/
├── MyResearchProject/
│   ├── .plotter_project
│   ├── Literature/
│   │   ├── .plotter_folder
│   │   ├── paper1.md
│   │   ├── paper1.md.plotter_meta
│   │   └── Notes/
│   │       ├── .plotter_folder
│   │       ├── summary.md
│   │       └── summary.md.plotter_meta
│   └── Ideas/
│       ├── .plotter_folder
│       ├── brainstorm.md
│       └── brainstorm.md.plotter_meta
└── PersonalProject/
    └── .plotter_project
```

## Metadata Format

### Project Metadata (.plotter_project)
```json
{
   "id": "uuid-here",
   "name": "MyResearchProject",
   "description": "My research notes",
   "createdAt": 1730400000000,
   "updatedAt": 1730400000000,
   "folderIds": ["folder-uuid-1", "folder-uuid-2"]
}
```

### Folder Metadata (.plotter_folder)
```json
{
   "id": "folder-uuid",
   "name": "Literature",
   "description": "Research papers",
   "parentProjectId": "project-uuid",
   "parentFolderId": "",
   "createdAt": 1730400000000,
   "updatedAt": 1730400000000,
   "noteIds": ["note-uuid-1"],
   "subfolderIds": ["subfolder-uuid"]
}
```

### Note Metadata (filename.md.plotter_meta)
```json
{
   "id": "note-uuid",
   "name": "paper1",
   "parentFolderId": "folder-uuid",
   "createdAt": 1730400000000,
   "updatedAt": 1730400000000
}
```

## Advantages

✅ **Human-readable** - All data in plain text
✅ **Git-friendly** - Easy version control
✅ **No database** - No SQLite dependency for this backend
✅ **Portable** - Just copy the folder
✅ **Debuggable** - Open and edit metadata files directly
✅ **Tool-agnostic** - Use any text editor

## Considerations

⚠️ **Performance** - Slower than SQLite for large datasets
⚠️ **Concurrency** - Limited multi-process support
⚠️ **Scale** - Best for human-scale projects (100s-1000s of notes)

## Next Steps

1. Build all three modules
2. Run the tests to verify everything works
3. Integrate into your CLI (see integration pattern above)
4. Add a command-line flag to choose between SQLite and filesystem backends
5. Enjoy your file-based Plotter storage!

## Troubleshooting

### Build Errors
- Ensure jsoncpp is installed
- Check that CMake can find all dependencies
- Verify C++17 support in your compiler

### Runtime Errors
- Check file permissions on the root directory
- Ensure the path exists and is writable
- Look for JSON parsing errors in metadata files

## Support

For questions or issues, please refer to the README files in each module directory:
- `PlotterFilesystemDTOs/README.md`
- `PlotterFilesystemMappers/README.md`
- `PlotterFilesystemDataSource/README.md`

