# PlotterFilesystemMappers

This module provides mappers that convert between domain entities and filesystem-based Data Transfer Objects (DTOs) for the Plotter project.

## Overview

The FilesystemMappers module implements the mapper pattern to bridge the gap between:
- **Domain Layer**: Entity objects (Project, Folder, Note)
- **Infrastructure Layer**: FilesystemDTOs that represent data stored in the filesystem

## Architecture

```
Domain Entities (Project, Folder, Note)
           ↕
  FilesystemMappers (this module)
           ↕
FilesystemDTOs (filesystem representation)
           ↕
FilesystemDataSource (actual file I/O)
```

## Mappers

- **FilesystemProjectMapper**: Converts between `Project` entities and `FilesystemProjectDTO`
- **FilesystemFolderMapper**: Converts between `Folder` entities and `FilesystemFolderDTO`
- **FilesystemNoteMapper**: Converts between `Note` entities and `FilesystemNoteDTO`

## Usage

Mappers are used internally by the repository layer and should not be accessed directly by application code. They are injected into repositories during initialization.

## Building

```bash
cd PlotterFilesystemMappers
mkdir -p build && cd build
cmake ..
make
```

## Dependencies

- PlotterDTOs (base DTO interfaces)
- PlotterFilesystemDTOs (filesystem-specific DTOs)
- PlotterRepositories (mapper interfaces - external)
- PlotterEntities (domain entities - external)

