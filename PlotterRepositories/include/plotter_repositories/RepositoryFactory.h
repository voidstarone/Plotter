#ifndef REPOSITORY_FACTORY_H
#define REPOSITORY_FACTORY_H

#include "ProjectDataSource.h"
#include "FolderDataSource.h"
#include "NoteDataSource.h"
#include "DataSourceRouter.h"
#include "RoutingStrategy.h"
#include <memory>
#include <string>
#include <map>

namespace plotter {
namespace repositories {

/**
 * @brief Configuration for datasource setup
 */
struct DataSourceConfig {
    std::string type;                           // e.g., "SQLite", "Memory", "FileSystem"
    std::string name;                           // Unique identifier
    int priority{0};                            // Priority for routing
    std::map<std::string, std::string> params;  // Type-specific parameters
    
    DataSourceConfig() = default;
    DataSourceConfig(const std::string& type, const std::string& name, int priority = 0)
        : type(type), name(name), priority(priority) {}
};

/**
 * @brief Configuration for routing strategy
 */
struct RoutingConfig {
    RoutingStrategyType type{RoutingStrategyType::PRIORITY_BASED};
    std::map<std::string, std::string> params;  // Strategy-specific parameters
    
    RoutingConfig() = default;
    explicit RoutingConfig(RoutingStrategyType type) : type(type) {}
};

/**
 * @brief Factory interface for creating datasources and routers
 * 
 * Concrete implementations will create specific datasource instances
 * (e.g., SQLite, Memory, FileSystem) based on configuration.
 * 
 * This is an interface that testing implementations must provide.
 */
class RepositoryFactory {
public:
    virtual ~RepositoryFactory() = default;
    
    /**
     * @brief Create a Project datasource
     * 
     * @param config Configuration for the datasource
     * @return Pointer to the created datasource (ownership transferred to caller)
     */
    virtual ProjectDataSource* createProjectDataSource(const DataSourceConfig& config) = 0;
    
    /**
     * @brief Create a Folder datasource
     * 
     * @param config Configuration for the datasource
     * @return Pointer to the created datasource (ownership transferred to caller)
     */
    virtual FolderDataSource* createFolderDataSource(const DataSourceConfig& config) = 0;
    
    /**
     * @brief Create a Note datasource
     * 
     * @param config Configuration for the datasource
     * @return Pointer to the created datasource (ownership transferred to caller)
     */
    virtual NoteDataSource* createNoteDataSource(const DataSourceConfig& config) = 0;
    
    /**
     * @brief Create a routing strategy
     * 
     * @param config Configuration for the routing strategy
     * @return Pointer to the created strategy (ownership transferred to caller)
     */
    virtual RoutingStrategy* createRoutingStrategy(const RoutingConfig& config) = 0;
    
    /**
     * @brief Create a router for Project datasources
     * 
     * @param configs Vector of datasource configurations
     * @param routingConfig Routing strategy configuration
     * @return Pointer to the configured router (ownership transferred to caller)
     */
    virtual DataSourceRouter<ProjectDataSource>* createProjectRouter(
        const std::vector<DataSourceConfig>& configs,
        const RoutingConfig& routingConfig) = 0;
    
    /**
     * @brief Create a router for Folder datasources
     * 
     * @param configs Vector of datasource configurations
     * @param routingConfig Routing strategy configuration
     * @return Pointer to the configured router (ownership transferred to caller)
     */
    virtual DataSourceRouter<FolderDataSource>* createFolderRouter(
        const std::vector<DataSourceConfig>& configs,
        const RoutingConfig& routingConfig) = 0;
    
    /**
     * @brief Create a router for Note datasources
     * 
     * @param configs Vector of datasource configurations
     * @param routingConfig Routing strategy configuration
     * @return Pointer to the configured router (ownership transferred to caller)
     */
    virtual DataSourceRouter<NoteDataSource>* createNoteRouter(
        const std::vector<DataSourceConfig>& configs,
        const RoutingConfig& routingConfig) = 0;
    
    /**
     * @brief Perform health check on a datasource
     * 
     * @param datasource Pointer to the datasource
     * @return Health check result
     */
    virtual HealthCheckResult performHealthCheck(DataSource* datasource) = 0;
    
    /**
     * @brief Get factory name/type
     * 
     * @return Name of the factory (e.g., "TestFactory", "ProductionFactory")
     */
    virtual std::string getFactoryType() const = 0;
};

} // namespace repositories
} // namespace plotter

#endif // REPOSITORY_FACTORY_H