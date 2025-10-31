#ifndef PLOTTER_CONFIG_H
#define PLOTTER_CONFIG_H

/**
 * @file PlotterConfig.h
 * @brief Compile-time configuration for Plotter data sources
 *
 * This file defines macros that control which data source implementations
 * are compiled into the application. These macros should be set by the
 * build system (CMake) based on build options.
 *
 * Usage in code:
 *
 * #ifdef PLOTTER_ENABLE_SQLITE
 *   #include "plotter_sqlite/SqliteProjectDataSource.h"
 * #endif
 *
 * #ifdef PLOTTER_ENABLE_FILESYSTEM
 *   #include "plotter_filesystem/FilesystemDataSource.h"
 * #endif
 */

// These macros are defined by CMake based on build options:
// - PLOTTER_ENABLE_SQLITE: Enable SQLite data source support
// - PLOTTER_ENABLE_FILESYSTEM: Enable Filesystem data source support

// Ensure at least one data source is enabled
#if !defined(PLOTTER_ENABLE_SQLITE) && !defined(PLOTTER_ENABLE_FILESYSTEM)
    #error "At least one data source must be enabled (PLOTTER_ENABLE_SQLITE or PLOTTER_ENABLE_FILESYSTEM)"
#endif

// Version information
#define PLOTTER_VERSION_MAJOR 1
#define PLOTTER_VERSION_MINOR 0
#define PLOTTER_VERSION_PATCH 0

// Feature flags
#ifdef PLOTTER_ENABLE_SQLITE
    #define PLOTTER_HAS_SQLITE 1
#else
    #define PLOTTER_HAS_SQLITE 0
#endif

#ifdef PLOTTER_ENABLE_FILESYSTEM
    #define PLOTTER_HAS_FILESYSTEM 1
#else
    #define PLOTTER_HAS_FILESYSTEM 0
#endif

// Helper macros for conditional code
#define PLOTTER_DATASOURCE_COUNT (PLOTTER_HAS_SQLITE + PLOTTER_HAS_FILESYSTEM)

#endif // PLOTTER_CONFIG_H

