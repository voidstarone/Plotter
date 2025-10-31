#ifndef MOCK_FOLDER_DATASOURCE_H
#define MOCK_FOLDER_DATASOURCE_H

#include "plotter_repositories/FolderDataSource.h"
#include <map>
#include <chrono>

using namespace plotter::repositories;

/**
 * @brief Mock in-memory implementation of FolderDataSource for testing
 */
class MockFolderDataSource : public FolderDataSource {
private:
    std::string name;
    std::string type;
    int priority;
    bool available;
    std::map<std::string, Folder> storage;
    DataSourceMetrics metrics;
    
public:
    MockFolderDataSource(const std::string& name, const std::string& type, int priority)
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
    
    // FolderDataSource interface
    std::string save(const Folder& folder) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        storage.insert_or_assign(folder.getId(), folder);
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return folder.getId();
    }
    
    std::optional<Folder> findById(const std::string& id) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        auto it = storage.find(id);
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return (it != storage.end()) ? std::optional<Folder>(it->second) : std::nullopt;
    }
    
    std::vector<Folder> findAll() override {
        if (!available) throw std::runtime_error("DataSource is not available");
        std::vector<Folder> result;
        for (const auto& [id, folder] : storage) {
            result.push_back(folder);
        }
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return result;
    }
    
    std::vector<Folder> findByProjectId(const std::string& projectId) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        std::vector<Folder> result;
        for (const auto& [id, folder] : storage) {
            if (folder.getParentProjectId() == projectId) {
                result.push_back(folder);
            }
        }
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return result;
    }
    
    std::vector<Folder> findByParentFolderId(const std::string& parentFolderId) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        std::vector<Folder> result;
        for (const auto& [id, folder] : storage) {
            if (folder.getParentFolderId() == parentFolderId) {
                result.push_back(folder);
            }
        }
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return result;
    }
    
    bool deleteById(const std::string& id) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        bool found = storage.erase(id) > 0;
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return found;
    }
    
    bool update(const Folder& folder) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        auto it = storage.find(folder.getId());
        if (it == storage.end()) return false;
        it->second = folder;
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return true;
    }
    
    bool exists(const std::string& id) override {
        if (!available) return false;
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

#endif // MOCK_FOLDER_DATASOURCE_H