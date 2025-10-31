#ifndef PLOTTER_REPOSITORIES_H
#define PLOTTER_REPOSITORIES_H

/**
 * @file PlotterRepositories.h
 * @brief Main header for Plotter Repositories Library
 * 
 * This library provides abstract interfaces for multi-source repository implementations
 * following Clean Architecture principles. It defines:
 * 
 * - DataSource interfaces for different storage backends
 * - Routing strategies for intelligent datasource selection
 * - Router interfaces for managing multiple datasources
 * - Factory interfaces for creating and configuring datasources
 * 
 * This library contains ONLY INTERFACES. Concrete implementations should be
 * provided by datasource-specific libraries (e.g., PlotterRepositoriesSQLite,
 * PlotterRepositoriesRedis, PlotterRepositoriesMemory, etc.)
 * 
 * Architecture:
 * 
 * ```
 * ┌─────────────────────────────────────────────────────────┐
 * │                    Use Cases Layer                      │
 * │  (PlotterUseCases - depends on repository interfaces)   │
 * └─────────────────────────────────────────────────────────┘
 *                            │
 *                            ▼
 * ┌─────────────────────────────────────────────────────────┐
 * │              Repository Interfaces Layer                │
 * │         (This library - PlotterRepositories)            │
 * │  ┌──────────────────────────────────────────────────┐   │
 * │  │  DataSource Interfaces (ProjectDataSource, etc) │   │
 * │  │  Routing Strategies (PriorityBased, CacheFirst) │   │
 * │  │  Router Interface (DataSourceRouter<T>)         │   │
 * │  │  Factory Interface (RepositoryFactory)          │   │
 * │  └──────────────────────────────────────────────────┘   │
 * └─────────────────────────────────────────────────────────┘
 *                            │
 *                            ▼
 * ┌─────────────────────────────────────────────────────────┐
 * │           Concrete Implementations Layer                │
 * │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
 * │  │   SQLite     │  │    Memory    │  │  FileSystem  │  │
 * │  │ DataSources  │  │ DataSources  │  │ DataSources  │  │
 * │  │  + DTOs      │  │  + DTOs      │  │  + DTOs      │  │
 * │  │  + Mappers   │  │  + Mappers   │  │  + Mappers   │  │
 * │  └──────────────┘  └──────────────┘  └──────────────┘  │
 * └─────────────────────────────────────────────────────────┘
 * ```
 * 
 * Usage Example:
 * 
 * ```cpp
 * // Implementation library provides concrete factory
 * auto factory = std::make_unique<TestRepositoryFactory>();
 * 
 * // Configure datasources
 * DataSourceConfig sqliteConfig("SQLite", "main-db", 100);
 * DataSourceConfig memoryConfig("Memory", "cache", 200);
 * 
 * // Create router with cache-first strategy
 * RoutingConfig routingConfig(RoutingStrategyType::CACHE_FIRST);
 * auto router = factory->createProjectRouter(
 *     {sqliteConfig, memoryConfig}, routingConfig);
 * 
 * // Use router with use cases
 * CreateProjectUseCase createProject(router);
 * createProject.execute(name, description);
 * ```
 */

// Core interfaces
#include "DataSource.h"
#include "ProjectDataSource.h"
#include "FolderDataSource.h"
#include "NoteDataSource.h"

// Routing
#include "RoutingStrategy.h"
#include "DataSourceRouter.h"

// Factory
#include "RepositoryFactory.h"

// Repository implementations (uses DataSources to implement PlotterUseCases repository interfaces)
#include "MultiSourceProjectRepository.h"
#include "MultiSourceFolderRepository.h"
#include "MultiSourceNoteRepository.h"

#endif // PLOTTER_REPOSITORIES_H