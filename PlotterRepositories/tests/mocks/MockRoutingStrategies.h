#ifndef MOCK_ROUTING_STRATEGIES_H
#define MOCK_ROUTING_STRATEGIES_H

#include "plotter_repositories/RoutingStrategy.h"
#include <algorithm>

using namespace plotter::repositories;

/**
 * @brief Simple priority-based routing implementation for testing
 */
class SimplePriorityStrategy : public PriorityBasedStrategy {
public:
    DataSource* selectForRead(const std::vector<DataSource*>& availableSources) override {
        if (availableSources.empty()) return nullptr;
        
        // Sort by priority (highest first) and return the first available
        auto maxPriority = std::max_element(availableSources.begin(), availableSources.end(),
            [](DataSource* a, DataSource* b) {
                return a->getPriority() < b->getPriority();
            });
        
        return (maxPriority != availableSources.end()) ? *maxPriority : nullptr;
    }
    
    std::vector<DataSource*> selectForWrite(const std::vector<DataSource*>& availableSources) override {
        // Write to all available sources
        return availableSources;
    }
    
    void recordResult(DataSource* source, bool success, double responseTimeMs) override {
        // Simple implementation doesn't use this
        (void)source;
        (void)success;
        (void)responseTimeMs;
    }
};

/**
 * @brief Simple cache-first routing implementation for testing
 */
class SimpleCacheFirstStrategy : public CacheFirstStrategy {
private:
    std::vector<std::string> cacheTypes = {"Memory", "Cache"};
    bool writeThroughEnabled = true;
    
public:
    DataSource* selectForRead(const std::vector<DataSource*>& availableSources) override {
        if (availableSources.empty()) return nullptr;
        
        // First try cache types
        for (auto* source : availableSources) {
            for (const auto& cacheType : cacheTypes) {
                if (source->getType() == cacheType) {
                    return source;
                }
            }
        }
        
        // Fallback to first available source
        return availableSources[0];
    }
    
    std::vector<DataSource*> selectForWrite(const std::vector<DataSource*>& availableSources) override {
        if (!writeThroughEnabled) {
            // Write only to persistent storage (non-cache)
            std::vector<DataSource*> result;
            for (auto* source : availableSources) {
                bool isCache = false;
                for (const auto& cacheType : cacheTypes) {
                    if (source->getType() == cacheType) {
                        isCache = true;
                        break;
                    }
                }
                if (!isCache) result.push_back(source);
            }
            return result;
        }
        
        // Write-through: write to all sources
        return availableSources;
    }
    
    void recordResult(DataSource* source, bool success, double responseTimeMs) override {
        (void)source; (void)success; (void)responseTimeMs;
    }
    
    void setCacheTypes(const std::vector<std::string>& types) override {
        cacheTypes = types;
    }
    
    void setWriteThroughEnabled(bool enabled) override {
        writeThroughEnabled = enabled;
    }
};

#endif // MOCK_ROUTING_STRATEGIES_H