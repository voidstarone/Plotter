#ifndef PLOTTER_ENTITY_STUBS_H
#define PLOTTER_ENTITY_STUBS_H

/**
 * @file EntityStubs.h
 * @brief DEPRECATED - Use real entities from PlotterEntities instead
 *
 * This file previously contained stub entities for backend-only builds.
 * This violated Clean Architecture by creating duplicate entity definitions.
 *
 * MIGRATION PATH:
 * 1. Remove #include "EntityStubs.h" from your code
 * 2. Add proper includes for real entities:
 *    #include "Project.h"
 *    #include "Folder.h"
 *    #include "Note.h"
 * 3. Update your CMakeLists.txt to link against PlotterEntities
 * 4. Remove the plotter:: namespace prefix (entities are in global namespace)
 *
 * This file will be removed in a future version.
 */

#error "EntityStubs.h is deprecated and removed. Use real entities from PlotterEntities: #include \"Project.h\", \"Folder.h\", \"Note.h\""

// The duplicate entity definitions have been removed to fix Clean Architecture violation.
// All code must now use the real entities from PlotterEntities package.

#endif // PLOTTER_ENTITY_STUBS_H
