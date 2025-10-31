#ifndef ROUTING_STRATEGY_H
#define ROUTING_STRATEGY_H

#include "DataSource.h"
#include <vector>
#include <memory>
#include <string>

namespace plotter {
namespace repositories {

/**
 * @brief Enum defining different routing strategies
 */
enum class RoutingStrategyType {
    PRIORITY_BASED,      // Use highest priority available source
    PERFORMANCE_BASED,   // Route to fastest source based on metrics
    CACHE_FIRST,         // Try cache, fallback to database
    LOAD_BALANCED,       // Distribute load across available sources
    FAILOVER,            // Use primary, failover to backup
    ROUND_ROBIN          // Distribute requests evenly
};

/**
 * @brief Base interface for routing strategies
 * 
 * A routing strategy determines which datasource(s) to use
 * for a given operation based on various criteria.
 */
class RoutingStrategy {
public:
    virtual ~RoutingStrategy() = default;
    
    /**
     * @brief Get the type of this routing strategy
     */
    virtual RoutingStrategyType getType() const = 0;
    
    /**
     * @brief Select a datasource for a read operation
     * 
     * @param availableSources List of available datasources
     * @return Pointer to the selected datasource, or nullptr if none available
     */
    virtual DataSource* selectForRead(const std::vector<DataSource*>& availableSources) = 0;
    
    /**
     * @brief Select datasources for a write operation
     * 
     * Write operations may target multiple datasources (e.g., write-through cache)
     * 
     * @param availableSources List of available datasources
     * @return Vector of datasources to write to, in order
     */
    virtual std::vector<DataSource*> selectForWrite(const std::vector<DataSource*>& availableSources) = 0;
    
    /**
     * @brief Notify the strategy of an operation result (for adaptive routing)
     * 
     * @param source The datasource that was used
     * @param success Whether the operation succeeded
     * @param responseTimeMs Response time in milliseconds
     */
    virtual void recordResult(DataSource* source, bool success, double responseTimeMs) = 0;
};

/**
 * @brief Priority-based routing strategy
 * 
 * Selects datasources based on their priority value.
 * Higher priority sources are preferred.
 */
class PriorityBasedStrategy : public RoutingStrategy {
public:
    virtual ~PriorityBasedStrategy() = default;
    
    RoutingStrategyType getType() const override {
        return RoutingStrategyType::PRIORITY_BASED;
    }
};

/**
 * @brief Performance-based routing strategy
 * 
 * Routes to the datasource with the best performance metrics
 * (lowest latency, highest success rate, etc.)
 */
class PerformanceBasedStrategy : public RoutingStrategy {
public:
    virtual ~PerformanceBasedStrategy() = default;
    
    RoutingStrategyType getType() const override {
        return RoutingStrategyType::PERFORMANCE_BASED;
    }
    
    /**
     * @brief Set the weight for response time in performance calculation
     * 
     * @param weight Weight value (0.0 to 1.0)
     */
    virtual void setResponseTimeWeight(double weight) = 0;
    
    /**
     * @brief Set the weight for success rate in performance calculation
     * 
     * @param weight Weight value (0.0 to 1.0)
     */
    virtual void setSuccessRateWeight(double weight) = 0;
};

/**
 * @brief Cache-first routing strategy
 * 
 * Attempts to read from cache first, falls back to database on miss.
 * Writes go to both cache and database (write-through).
 */
class CacheFirstStrategy : public RoutingStrategy {
public:
    virtual ~CacheFirstStrategy() = default;
    
    RoutingStrategyType getType() const override {
        return RoutingStrategyType::CACHE_FIRST;
    }
    
    /**
     * @brief Set which datasource types should be considered as cache
     * 
     * @param cacheTypes Vector of datasource type names (e.g., "Redis", "Memory")
     */
    virtual void setCacheTypes(const std::vector<std::string>& cacheTypes) = 0;
    
    /**
     * @brief Enable or disable write-through to cache
     * 
     * @param enabled If true, writes go to both cache and persistent storage
     */
    virtual void setWriteThroughEnabled(bool enabled) = 0;
};

/**
 * @brief Load-balanced routing strategy
 * 
 * Distributes requests across multiple datasources to balance load.
 */
class LoadBalancedStrategy : public RoutingStrategy {
public:
    virtual ~LoadBalancedStrategy() = default;
    
    RoutingStrategyType getType() const override {
        return RoutingStrategyType::LOAD_BALANCED;
    }
    
    /**
     * @brief Set the load balancing algorithm
     * 
     * @param algorithm "round-robin", "least-connections", "weighted"
     */
    virtual void setAlgorithm(const std::string& algorithm) = 0;
};

/**
 * @brief Failover routing strategy
 * 
 * Uses a primary datasource, automatically fails over to backup if primary is unavailable.
 */
class FailoverStrategy : public RoutingStrategy {
public:
    virtual ~FailoverStrategy() = default;
    
    RoutingStrategyType getType() const override {
        return RoutingStrategyType::FAILOVER;
    }
    
    /**
     * @brief Set the primary datasource type
     * 
     * @param primaryType Type name of the primary datasource
     */
    virtual void setPrimaryType(const std::string& primaryType) = 0;
    
    /**
     * @brief Set automatic failback when primary recovers
     * 
     * @param enabled If true, automatically return to primary when it becomes healthy
     */
    virtual void setAutoFailback(bool enabled) = 0;
};

} // namespace repositories
} // namespace plotter

#endif // ROUTING_STRATEGY_H