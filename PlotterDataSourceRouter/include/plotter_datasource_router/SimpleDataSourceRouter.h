#ifndef SIMPLE_DATASOURCE_ROUTER_H
#define SIMPLE_DATASOURCE_ROUTER_H

#include "plotter_repositories/DataSourceRouter.h"
#include <vector>
#include <stdexcept>

namespace plotter {
namespace datasource_router {

/**
 * @brief Simple implementation of DataSourceRouter that uses only one data source
 * 
 * This is a straightforward implementation that routes all operations to a single
 * data source. Useful for simple scenarios or testing.
 */
template<typename DataSourceType>
class SimpleDataSourceRouter : public repositories::DataSourceRouter<DataSourceType> {
private:
    DataSourceType* dataSource;
    
public:
    /**
     * @brief Construct a router with a single data source
     * 
     * @param ds The data source to use (ownership NOT transferred)
     */
    explicit SimpleDataSourceRouter(DataSourceType* ds) : dataSource(ds) {
        if (!ds) {
            throw std::invalid_argument("DataSource cannot be null");
        }
    }
    
    virtual ~SimpleDataSourceRouter() = default;
    
    // Implement abstract methods from DataSourceRouter
    void addDataSource(DataSourceType* datasource) override {
        // Simple router only uses one data source, ignore additions
    }
    
    bool removeDataSource(const std::string& datasourceName) override {
        return false; // Cannot remove the only data source
    }
    
    void setRoutingStrategy(repositories::RoutingStrategy* strategy) override {
        // Simple router doesn't use strategies
    }
    
    std::vector<DataSourceType*> getAllDataSources() const override {
        return {dataSource};
    }
    
    std::vector<DataSourceType*> getAvailableDataSources() const override {
        return {dataSource};
    }
    
    DataSourceType* getDataSource(const std::string& name) const override {
        return (dataSource->getName() == name) ? dataSource : nullptr;
    }
    
    DataSourceType* selectForRead() override {
        return dataSource;
    }
    
    std::vector<DataSourceType*> selectForWrite() override {
        return {dataSource};
    }
    
    std::vector<std::pair<std::string, repositories::HealthCheckResult>> checkAllHealth() override {
        return {{dataSource->getName(), dataSource->checkHealth()}};
    }
    
    // Template methods for executing operations
    template<typename R>
    R executeRead(std::function<R(DataSourceType*)> operation) {
        if (!dataSource) {
            throw std::runtime_error("No datasource available for read");
        }
        return operation(dataSource);
    }
    
    template<typename R>
    std::vector<R> executeWrite(std::function<R(DataSourceType*)> operation) {
        if (!dataSource) {
            throw std::runtime_error("No datasource available for write");
        }
        return {operation(dataSource)};
    }
};

} // namespace datasource_router
} // namespace plotter

#endif // SIMPLE_DATASOURCE_ROUTER_H
