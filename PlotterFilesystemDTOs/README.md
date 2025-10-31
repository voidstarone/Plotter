# PlotterFilesystemDTOs

This module provides Data Transfer Objects (DTOs) for the filesystem-based data source implementation in the Plotter project.

## Overview

The FilesystemDTOs module defines DTOs that represent how Projects, Folders, and Notes are stored in the filesystem:

- **Projects**: Top-level directories with a hidden `.plotter_project` file containing metadata
- **Folders**: Subdirectories with a hidden `.plotter_folder` file containing metadata  
- **Notes**: Regular text/markdown files with a companion `.plotter_meta` file containing metadata

## Directory Structure

```
project-root/
├── .plotter_project          # Contains project ID, description, timestamps
├── folder1/
│   ├── .plotter_folder       # Contains folder ID, parent relationships
│   ├── note1.md
│   ├── note1.md.plotter_meta # Contains note ID, timestamps
│   └── subfolder/
│       └── .plotter_folder
└── folder2/
    └── .plotter_folder
```

## Features

- **Hidden Dotfiles**: Metadata stored in hidden files (starting with `.plotter_`)
- **UUID-based IDs**: Each entity gets a unique UUID stored in its dotfile
- **Timestamp Tracking**: Creation and update timestamps stored in milliseconds
- **Filesystem-Native**: Uses actual directories and files for natural organization

## Building

```bash
cd PlotterFilesystemDTOs
mkdir -p build && cd build
cmake ..
make
```

## Usage

The DTOs are used internally by the FilesystemDataSource implementation and should not be accessed directly by application code. The repository layer uses mappers to convert between these DTOs and domain entities.

