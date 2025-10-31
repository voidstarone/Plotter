#ifndef DATASOURCE_ROUTER_H
#define DATASOURCE_ROUTER_H

#include "DataSource.h"
#include "RoutingStrategy.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace plotter {
namespace repositories {

/**
 * @brief Abstract router interface that manages multiple datasources and routes operations
 * 
 * The DataSourceRouter maintains a collection of datasources and uses
 * a routing strategy to determine which datasource(s) to use for each operation.
 * 
 * Concrete implementations will be provided by separate routing packages.
 * 
 * Template parameter T should be a datasource interface type (e.g., ProjectDataSource)
 */
template<typename T>
class DataSourceRouter {
public:
    virtual ~DataSourceRouter() = default;
    
    /**
     * @brief Add a datasource to the router
     * 
     * @param datasource Pointer to the datasource to add
     */
    virtual void addDataSource(T* datasource) = 0;
    
    /**
     * @brief Remove a datasource from the router
     * 
     * @param datasourceName Name of the datasource to remove
     * @return True if removed, false if not found
     */
    virtual bool removeDataSource(const std::string& datasourceName) = 0;
    
    /**
     * @brief Set the routing strategy
     * 
     * @param strategy Pointer to the routing strategy to use
     */
    virtual void setRoutingStrategy(RoutingStrategy* strategy) = 0;
    
    /**
     * @brief Get all registered datasources
     * 
     * @return Vector of pointers to all datasources
     */
    virtual std::vector<T*> getAllDataSources() const = 0;
    
    /**
     * @brief Get all available (healthy) datasources
     * 
     * @return Vector of pointers to available datasources
     */
    virtual std::vector<T*> getAvailableDataSources() const = 0;
    
    /**
     * @brief Get a datasource by name
     * 
     * @param name Name of the datasource
     * @return Pointer to the datasource, or nullptr if not found
     */
    virtual T* getDataSource(const std::string& name) const = 0;
    
    /**
     * @brief Select a datasource for a read operation using the routing strategy
     * 
     * @return Pointer to the selected datasource, or nullptr if none available
     */
    virtual T* selectForRead() = 0;
    
    /**
     * @brief Select datasources for a write operation using the routing strategy
     * 
     * @return Vector of datasources to write to, in order
     */
    virtual std::vector<T*> selectForWrite() = 0;
    
    /**
     * @brief Perform health check on all datasources
     * 
     * @return Map of datasource names to their health check results
     */
    virtual std::vector<std::pair<std::string, HealthCheckResult>> checkAllHealth() = 0;
    
    /**
     * @brief Execute a read operation with automatic fallback
     * 
     * Attempts the operation on the selected datasource, falling back to
     * other available sources if it fails.
     * 
     * NOTE: This is a template method that cannot be virtual.
     * Concrete router implementations must provide this method.
     * 
     * @param operation Function to execute on a datasource
     * @return Result of the operation
     * @throws std::runtime_error if all datasources fail
     */
    template<typename R>
    R executeRead(std::function<R(T*)> operation) = delete;
    
    /**
     * @brief Execute a write operation on all selected datasources
     * 
     * NOTE: This is a template method that cannot be virtual.
     * Concrete router implementations must provide this method.
     * 
     * @param operation Function to execute on each datasource
     * @return Vector of results from each datasource
     */
    template<typename R>
    std::vector<R> executeWrite(std::function<R(T*)> operation) = delete;
};

} // namespace repositories
} // namespace plotter

#endif // DATASOURCE_ROUTER_H