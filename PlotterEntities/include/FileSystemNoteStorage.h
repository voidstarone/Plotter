#ifndef FILESYSTEMNOTESTORAGE_H
#define FILESYSTEMNOTESTORAGE_H

// DEPRECATED: This file has been moved to PlotterFilesystemDataSource
// This is a compatibility shim to help with migration
//
// OLD LOCATION (WRONG - VIOLATES CLEAN ARCHITECTURE):
//   PlotterEntities/include/FileSystemNoteStorage.h
//
// NEW LOCATION (CORRECT - INFRASTRUCTURE LAYER):
//   PlotterFilesystemDataSource/include/plotter_filesystem/FilesystemNoteStorage.h
//
// Please update your includes to use the new location.

#warning "FileSystemNoteStorage.h is deprecated. Use plotter_filesystem/FilesystemNoteStorage.h instead. This file will be removed in a future version."

#include "plotter_filesystem/FilesystemNoteStorage.h"

// Provide backward compatibility alias
using FileSystemNoteStorage = plotter::filesystem::FilesystemNoteStorage;

#endif // FILESYSTEMNOTESTORAGE_H

