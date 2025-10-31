#ifndef MOCK_DATASOURCE_ROUTER_H
#define MOCK_DATASOURCE_ROUTER_H

#include "plotter_repositories/DataSourceRouter.h"
#include <stdexcept>
#include <sstream>

using namespace plotter::repositories;

/**
 * @brief Simple implementation of DataSourceRouter for testing
 */
template<typename T>
class SimpleDataSourceRouter : public DataSourceRouter<T> {
private:
    std::vector<T*> dataSources;
    RoutingStrategy* strategy;
    
public:
    SimpleDataSourceRouter() : strategy(nullptr) {}
    
    explicit SimpleDataSourceRouter(RoutingStrategy* strat) : strategy(strat) {}
    
    ~SimpleDataSourceRouter() override = default;
    
    void addDataSource(T* datasource) override {
        if (datasource) {
            dataSources.push_back(datasource);
        }
    }
    
    bool removeDataSource(const std::string& datasourceName) override {
        auto it = std::remove_if(dataSources.begin(), dataSources.end(),
            [&datasourceName](T* ds) { return ds->getName() == datasourceName; });
        
        if (it != dataSources.end()) {
            dataSources.erase(it, dataSources.end());
            return true;
        }
        return false;
    }
    
    void setRoutingStrategy(RoutingStrategy* strat) override {
        strategy = strat;
    }
    
    std::vector<T*> getAllDataSources() const override {
        return dataSources;
    }
    
    std::vector<T*> getAvailableDataSources() const override {
        std::vector<T*> available;
        for (auto* ds : dataSources) {
            if (ds->isAvailable()) {
                available.push_back(ds);
            }
        }
        return available;
    }
    
    T* getDataSource(const std::string& name) const override {
        for (auto* ds : dataSources) {
            if (ds->getName() == name) {
                return ds;
            }
        }
        return nullptr;
    }
    
    T* selectForRead() override {
        auto available = getAvailableDataSources();
        if (available.empty()) return nullptr;
        
        if (!strategy) {
            // Default: return first available
            return available[0];
        }
        
        // Convert to DataSource pointers for strategy
        std::vector<DataSource*> sources;
        for (auto* ds : available) {
            sources.push_back(static_cast<DataSource*>(ds));
        }
        
        auto* selected = strategy->selectForRead(sources);
        return static_cast<T*>(selected);
    }
    
    std::vector<T*> selectForWrite() override {
        auto available = getAvailableDataSources();
        if (available.empty()) return {};
        
        if (!strategy) {
            // Default: write to all available
            return available;
        }
        
        // Convert to DataSource pointers for strategy
        std::vector<DataSource*> sources;
        for (auto* ds : available) {
            sources.push_back(static_cast<DataSource*>(ds));
        }
        
        auto selected = strategy->selectForWrite(sources);
        
        // Convert back to T*
        std::vector<T*> result;
        for (auto* ds : selected) {
            result.push_back(static_cast<T*>(ds));
        }
        return result;
    }
    
    std::vector<std::pair<std::string, HealthCheckResult>> checkAllHealth() override {
        std::vector<std::pair<std::string, HealthCheckResult>> results;
        for (auto* ds : dataSources) {
            results.push_back({ds->getName(), ds->checkHealth()});
        }
        return results;
    }
    
    // Implementation of template methods
    template<typename R>
    R executeRead(std::function<R(T*)> operation) {
        auto available = getAvailableDataSources();
        
        if (available.empty()) {
            throw std::runtime_error("No available datasources for read operation");
        }
        
        // Try each available source in order
        std::ostringstream errors;
        for (auto* ds : available) {
            try {
                auto start = std::chrono::high_resolution_clock::now();
                R result = operation(ds);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> elapsed = end - start;
                
                if (strategy) {
                    strategy->recordResult(ds, true, elapsed.count());
                }
                
                return result;
            } catch (const std::exception& e) {
                if (strategy) {
                    strategy->recordResult(ds, false, 0.0);
                }
                errors << "[" << ds->getName() << ": " << e.what() << "] ";
                // Continue to next datasource
            }
        }
        
        // All datasources failed
        throw std::runtime_error("All datasources failed: " + errors.str());
    }
    
    template<typename R>
    std::vector<R> executeWrite(std::function<R(T*)> operation) {
        auto selected = selectForWrite();
        
        if (selected.empty()) {
            throw std::runtime_error("No available datasources for write operation");
        }
        
        std::vector<R> results;
        std::ostringstream errors;
        bool anySuccess = false;
        
        for (auto* ds : selected) {
            try {
                auto start = std::chrono::high_resolution_clock::now();
                R result = operation(ds);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> elapsed = end - start;
                
                if (strategy) {
                    strategy->recordResult(ds, true, elapsed.count());
                }
                
                results.push_back(result);
                anySuccess = true;
            } catch (const std::exception& e) {
                if (strategy) {
                    strategy->recordResult(ds, false, 0.0);
                }
                errors << "[" << ds->getName() << ": " << e.what() << "] ";
                // Continue to next datasource
            }
        }
        
        if (!anySuccess) {
            throw std::runtime_error("All datasources failed: " + errors.str());
        }
        
        return results;
    }
};

#endif // MOCK_DATASOURCE_ROUTER_H