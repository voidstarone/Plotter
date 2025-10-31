#ifndef MOCK_NOTE_DATASOURCE_H
#define MOCK_NOTE_DATASOURCE_H

#include "plotter_repositories/NoteDataSource.h"
#include <map>
#include <chrono>

using namespace plotter::repositories;

/**
 * @brief Mock in-memory implementation of NoteDataSource for testing
 */
class MockNoteDataSource : public NoteDataSource {
private:
    std::string name;
    std::string type;
    int priority;
    bool available;
    std::map<std::string, Note> storage;
    DataSourceMetrics metrics;
    
public:
    MockNoteDataSource(const std::string& name, const std::string& type, int priority)
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
    
    // NoteDataSource interface
    std::string save(const Note& note) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        storage.insert_or_assign(note.getId(), note);
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return note.getId();
    }
    
    std::optional<Note> findById(const std::string& id) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        auto it = storage.find(id);
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return (it != storage.end()) ? std::optional<Note>(it->second) : std::nullopt;
    }
    
    std::vector<Note> findAll() override {
        if (!available) throw std::runtime_error("DataSource is not available");
        std::vector<Note> result;
        for (const auto& [id, note] : storage) {
            result.push_back(note);
        }
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return result;
    }
    
    std::vector<Note> findByParentFolderId(const std::string& parentFolderId) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        std::vector<Note> result;
        for (const auto& [id, note] : storage) {
            if (note.getParentFolderId() == parentFolderId) {
                result.push_back(note);
            }
        }
        metrics.totalRequests++;
        metrics.successfulRequests++;
        return result;
    }
    
    std::vector<Note> search(const std::string& searchTerm) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        std::vector<Note> result;
        for (const auto& [id, note] : storage) {
            // Simple search in name
            if (note.getName().find(searchTerm) != std::string::npos) {
                result.push_back(note);
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
    
    bool update(const Note& note) override {
        if (!available) throw std::runtime_error("DataSource is not available");
        auto it = storage.find(note.getId());
        if (it == storage.end()) return false;
        it->second = note;
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

#endif // MOCK_NOTE_DATASOURCE_H