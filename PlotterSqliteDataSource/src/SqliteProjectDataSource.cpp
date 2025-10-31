#include "plotter_sqlite/SqliteProjectDataSource.h"

namespace plotter {
namespace sqlite {

SqliteProjectDataSource::SqliteProjectDataSource(const std::string& name, const std::string& dbPath, int priority)
    : name(name), priority(priority), database(std::make_shared<SqliteDatabase>(dbPath)), available(false) {}

std::string SqliteProjectDataSource::getName() const {
    return name;
}

std::string SqliteProjectDataSource::getType() const {
    return "SQLite";
}

int SqliteProjectDataSource::getPriority() const {
    return priority;
}

bool SqliteProjectDataSource::isAvailable() const {
    return available && database && database->isConnected();
}

plotter::repositories::HealthCheckResult SqliteProjectDataSource::checkHealth() {
    using namespace plotter::repositories;
    
    HealthCheckResult result;
    result.checkTime = std::chrono::system_clock::now();
    result.metrics = metrics;

    if (!database || !database->isConnected()) {
        result.status = HealthStatus::UNHEALTHY;
        result.message = "Database is not connected";
        return result;
    }

    try {
        SqliteStatement stmt(database->getHandle(), "SELECT COUNT(*) FROM projects;");
        if (stmt.step() == SQLITE_ROW) {
            result.status = HealthStatus::HEALTHY;
            result.message = "SQLite datasource is operational";
        } else {
            result.status = HealthStatus::DEGRADED;
            result.message = "Database query failed";
        }
    } catch (const std::exception& e) {
        result.status = HealthStatus::UNHEALTHY;
        result.message = std::string("Health check failed: ") + e.what();
    }

    return result;
}

plotter::repositories::DataSourceMetrics SqliteProjectDataSource::getMetrics() const {
    return metrics;
}

void SqliteProjectDataSource::connect() {
    try {
        database->connect();
        available = true;
    } catch (const std::exception& e) {
        available = false;
        throw;
    }
}

void SqliteProjectDataSource::disconnect() {
    database->disconnect();
    available = false;
}

std::string SqliteProjectDataSource::save(const plotter::dto::ProjectDTO& projectDTO) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        // Cast to concrete SQLite DTO
        const sqlite_dtos::SqliteProjectDTO& dto = dynamic_cast<const sqlite_dtos::SqliteProjectDTO&>(projectDTO);

        const char* sql = R"(
            INSERT INTO projects (id, name, description, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?)
            ON CONFLICT(id) DO UPDATE SET
                name = excluded.name,
                description = excluded.description,
                updated_at = excluded.updated_at;
        )";

        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, dto.id);
        stmt.bindString(2, dto.name);
        stmt.bindString(3, dto.description);
        stmt.bindInt64(4, dto.createdAt);
        stmt.bindInt64(5, dto.updatedAt);

        if (!stmt.execute()) {
            throw std::runtime_error("Failed to save project");
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return dto.id;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

std::optional<plotter::dto::ProjectDTO*> SqliteProjectDataSource::findById(const std::string& id) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        // Query project
        const char* sql = "SELECT id, name, description, created_at, updated_at FROM projects WHERE id = ?;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, id);

        std::optional<plotter::dto::ProjectDTO*> result;
        if (stmt.step() == SQLITE_ROW) {
            result = rowToDTO(stmt);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return result;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

std::vector<plotter::dto::ProjectDTO*> SqliteProjectDataSource::findAll() {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = "SELECT id, name, description, created_at, updated_at FROM projects;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        std::vector<plotter::dto::ProjectDTO*> projects;

        while (stmt.step() == SQLITE_ROW) {
            projects.push_back(rowToDTO(stmt));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return projects;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

bool SqliteProjectDataSource::deleteById(const std::string& id) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = "DELETE FROM projects WHERE id = ?;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, id);
        
        bool success = stmt.execute();
        int changes = sqlite3_changes(database->getHandle());

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return success && changes > 0;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

bool SqliteProjectDataSource::update(const plotter::dto::ProjectDTO& projectDTO) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        // Cast to concrete SQLite DTO
        const sqlite_dtos::SqliteProjectDTO& dto = dynamic_cast<const sqlite_dtos::SqliteProjectDTO&>(projectDTO);

        if (!exists(dto.id)) {
            return false;
        }

        const char* sql = R"(
            UPDATE projects 
            SET name = ?, description = ?, updated_at = ?
            WHERE id = ?;
        )";

        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, dto.name);
        stmt.bindString(2, dto.description);
        stmt.bindInt64(3, dto.updatedAt);
        stmt.bindString(4, dto.id);

        bool success = stmt.execute();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return success;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

bool SqliteProjectDataSource::exists(const std::string& id) {
    try {
        if (!isAvailable()) {
            return false;
        }

        const char* sql = "SELECT 1 FROM projects WHERE id = ? LIMIT 1;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, id);

        return stmt.step() == SQLITE_ROW;
    } catch (const std::exception&) {
        return false;
    }
}

size_t SqliteProjectDataSource::clear() {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        SqliteStatement countStmt(database->getHandle(), "SELECT COUNT(*) FROM projects;");
        size_t count = 0;
        if (countStmt.step() == SQLITE_ROW) {
            count = countStmt.getColumnInt(0);
        }

        const char* sql = "DELETE FROM projects;";
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.execute();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return count;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

// Helper methods

void SqliteProjectDataSource::updateMetrics(bool success, double responseTimeMs) {
    metrics.totalRequests++;
    if (success) {
        metrics.successfulRequests++;
    } else {
        metrics.failedRequests++;
    }
    
    metrics.lastResponseTimeMs = responseTimeMs;
    metrics.averageResponseTimeMs = 
        (metrics.averageResponseTimeMs * (metrics.totalRequests - 1) + responseTimeMs) / metrics.totalRequests;
    metrics.lastAccessTime = std::chrono::system_clock::now();
}

sqlite_dtos::SqliteProjectDTO* SqliteProjectDataSource::rowToDTO(SqliteStatement& stmt) {
    sqlite_dtos::SqliteProjectDTO* dto = new sqlite_dtos::SqliteProjectDTO();
    dto->id = stmt.getColumnString(0);
    dto->name = stmt.getColumnString(1);
    dto->description = stmt.getColumnString(2);
    dto->createdAt = stmt.getColumnInt64(3);
    dto->updatedAt = stmt.getColumnInt64(4);
    return dto;
}

std::vector<std::string> SqliteProjectDataSource::getFolderIdsByProjectId(const std::string& projectId) {
    std::vector<std::string> folderIds;
    
    const char* sql = "SELECT id FROM folders WHERE parent_project_id = ?;";
    SqliteStatement stmt(database->getHandle(), sql);
    stmt.bindString(1, projectId);
    
    while (stmt.step() == SQLITE_ROW) {
        folderIds.push_back(stmt.getColumnString(0));
    }
    
    return folderIds;
}

} // namespace sqlite
} // namespace plotter
