#ifndef DATASOURCE_HEALTH_H
#define DATASOURCE_HEALTH_H

#include <string>
#include <chrono>

namespace plotter {
namespace repositories {

/**
 * @brief Health status of a datasource
 */
enum class HealthStatus {
    HEALTHY,        // DataSource is fully operational
    DEGRADED,       // DataSource is operational but with reduced performance
    UNHEALTHY,      // DataSource is not operational
    UNKNOWN         // Health status cannot be determined
};

/**
 * @brief Metrics for a datasource
 */
struct DataSourceMetrics {
    long long totalRequests{0};
    long long successfulRequests{0};
    long long failedRequests{0};
    double averageResponseTimeMs{0.0};
    double lastResponseTimeMs{0.0};
    std::chrono::system_clock::time_point lastAccessTime;
    
    double getSuccessRate() const {
        if (totalRequests == 0) return 0.0;
        return (static_cast<double>(successfulRequests) / totalRequests) * 100.0;
    }
    
    double getFailureRate() const {
        if (totalRequests == 0) return 0.0;
        return (static_cast<double>(failedRequests) / totalRequests) * 100.0;
    }
};

/**
 * @brief Health check result
 */
struct HealthCheckResult {
    HealthStatus status;
    std::string message;
    DataSourceMetrics metrics;
    std::chrono::system_clock::time_point checkTime;
    
    bool isHealthy() const {
        return status == HealthStatus::HEALTHY;
    }
    
    bool isAvailable() const {
        return status == HealthStatus::HEALTHY || status == HealthStatus::DEGRADED;
    }
};

/**
 * @brief Base interface for all data sources with health monitoring
 */
class DataSource {
public:
    virtual ~DataSource() = default;
    
    /**
     * @brief Get the name/identifier of this datasource
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get the type of this datasource (e.g., "SQLite", "Redis", "FileSystem")
     */
    virtual std::string getType() const = 0;
    
    /**
     * @brief Get the priority of this datasource (higher = more preferred)
     */
    virtual int getPriority() const = 0;
    
    /**
     * @brief Check if the datasource is currently available
     */
    virtual bool isAvailable() const = 0;
    
    /**
     * @brief Perform a health check on the datasource
     */
    virtual HealthCheckResult checkHealth() = 0;
    
    /**
     * @brief Get current metrics for this datasource
     */
    virtual DataSourceMetrics getMetrics() const = 0;
    
    /**
     * @brief Initialize/connect to the datasource
     */
    virtual void connect() = 0;
    
    /**
     * @brief Disconnect/cleanup the datasource
     */
    virtual void disconnect() = 0;
};

} // namespace repositories
} // namespace plotter

#endif // DATASOURCE_HEALTH_H