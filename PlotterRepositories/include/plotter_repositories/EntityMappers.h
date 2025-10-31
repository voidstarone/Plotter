#ifndef PLOTTER_REPOSITORIES_ENTITY_MAPPERS_H
#define PLOTTER_REPOSITORIES_ENTITY_MAPPERS_H

// This file has been deprecated in favor of concrete mappers like SqliteEntityMappers.
// The original design tried to map to abstract base DTOs which have no fields.
// Mapping should be done with concrete DTO types (SqliteProjectDTO, etc.)
// in the repository layer using concrete mappers.
//
// For SQLite datasources, use: plotter_repositories/SqliteEntityMappers.h
// For other datasources, create similar concrete mappers.

#warning "EntityMappers.h is deprecated - use SqliteEntityMappers.h or create concrete mappers for your datasource"

#endif // PLOTTER_REPOSITORIES_ENTITY_MAPPERS_H
