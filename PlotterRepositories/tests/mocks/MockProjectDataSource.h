#ifndef MOCK_PROJECT_DATASOURCE_H
#define MOCK_PROJECT_DATASOURCE_H

#include "plotter_repositories/ProjectDataSource.h"
#include <map>
#include <chrono>

using namespace plotter::repositories;

/**
 * @brief Mock in-memory implementation of ProjectDataSource for testing
 */
class MockProjectDataSource : public ProjectDataSource {
private:
    std::string name;
    std::string type;
    int priority;
    bool available;
    std::map<std::string, Project> storage;
    DataSourceMetrics metrics;
    
public:
    MockProjectDataSource(const std::string& name, const std::string& type, int priority)
        : name(name), type(type), priority(priority), available(true) {}
    
    // DataSource interface
    std::string getName() const override { return name; }
    std::string getType() const override { return type; }
    int getPriority() const override { return priority; }
    bool isAvailable() const override { return available; }
    
    HealthCheckResult checkHealth() override {
        HealthCheckResult result;
        result.status = available ? HealthStatus::HEALTHY : HealthStatus::UNHEALTHY;
        result.message = available ? "Mock datasource is healthy" : "Mock datasource is unavailable";
        result.metrics = metrics;
        result.checkTime = std::chrono::system_clock::now();
        return result;
    }
    
    DataSourceMetrics getMetrics() const override { return metrics; }
    
    void connect() override { available = true; }
    void disconnect() override { available = false; }
    
    // ProjectDataSource interface
    std::string save(const Project& project) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        storage.insert_or_assign(project.getId(), project);
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return project.getId();
    }
    
    std::optional<Project> findById(const std::string& id) override {
        auto start = std::chrono::high_resolution_clock::now();
        
        if (!available) {
            throw std::runtime_error("DataSource is not available");
        }
        
        auto it = storage.find(id);
        std::optional<Project> result = (it != storage.end()) 
            ? std::optional<Project>(it->second) 
            : std::nullopt;
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        
        metrics.totalRequests++;
        metrics.successfulRequests++;
        metrics.lastResponseTimeMs = elapsed.count();
        metrics.averageResponseTimeMs = 
            (metrics.averageResponseTimeMs * (metrics.totalRequests - 1) + elapsed.count()) / metrics.totalRequests;
        metrics.lastAccessTime = std::chrono::system_clock::now();
        
        return result;
    }
    
    std::vector<Project> findAll() override {
        if (!available) {
            throw std::runtime_error("DataSource is not available");
        }
        
        std::vector<Project> result;
        for (const auto& [id, project] : storage) {
            result.push_back(project);
        }
        
        metrics.totalRequests++;
        metrics.successfulRequests++;
        metrics.lastAccessTime = std::chrono::system_clock::now();
        
        return result;
    }
    
    bool deleteById(const std::string& id) override {
        if (!available) {
            throw std::runtime_error("DataSource is not available");
        }
        
        bool found = storage.erase(id) > 0;
        
        metrics.totalRequests++;
        metrics.successfulRequests++;
        metrics.lastAccessTime = std::chrono::system_clock::now();
        
        return found;
    }
    
    bool update(const Project& project) override {
        if (!available) {
            throw std::runtime_error("DataSource is not available");
        }
        
        auto it = storage.find(project.getId());
        if (it == storage.end()) {
            return false;
        }
        
        it->second = project;
        
        metrics.totalRequests++;
        metrics.successfulRequests++;
        metrics.lastAccessTime = std::chrono::system_clock::now();
        
        return true;
    }
    
    bool exists(const std::string& id) override {
        if (!available) {
            return false;
        }
        return storage.find(id) != storage.end();
    }
    
    size_t clear() override {
        size_t count = storage.size();
        storage.clear();
        return count;
    }
    
    // Test helpers
    void setAvailable(bool avail) { available = avail; }
    size_t getStorageSize() const { return storage.size(); }
};

#endif // MOCK_PROJECT_DATASOURCE_H