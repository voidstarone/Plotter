#include "plotter_sqlite/SqliteNoteDataSource.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
#include <iostream>
#include <sqlite3.h>

namespace plotter {
namespace sqlite {

SqliteNoteDataSource::SqliteNoteDataSource(const std::string& name, const std::string& dbPath, int priority)
    : name(name), priority(priority), database(std::make_shared<SqliteDatabase>(dbPath)), available(false) {}

std::string SqliteNoteDataSource::getName() const {
    return name;
}

std::string SqliteNoteDataSource::getType() const {
    return "SQLite";
}

int SqliteNoteDataSource::getPriority() const {
    return priority;
}

bool SqliteNoteDataSource::isAvailable() const {
    return available && database && database->isConnected();
}

plotter::repositories::HealthCheckResult SqliteNoteDataSource::checkHealth() {
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
        SqliteStatement stmt(database->getHandle(), "SELECT COUNT(*) FROM notes;");
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

plotter::repositories::DataSourceMetrics SqliteNoteDataSource::getMetrics() const {
    return metrics;
}

void SqliteNoteDataSource::connect() {
    try {
        database->connect();
        available = true;
    } catch (const std::exception& e) {
        available = false;
        throw;
    }
}

void SqliteNoteDataSource::disconnect() {
    database->disconnect();
    available = false;
}

std::string SqliteNoteDataSource::save(const plotter::dto::NoteDTO& noteDTO) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const sqlite_dtos::SqliteNoteDTO& dto = dynamic_cast<const sqlite_dtos::SqliteNoteDTO&>(noteDTO);

        const char* sql = R"(
            INSERT INTO notes (id, name, path, content, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
            ON CONFLICT(id) DO UPDATE SET
                name = excluded.name,
                path = excluded.path,
                content = excluded.content,
                parent_folder_id = excluded.parent_folder_id,
                updated_at = excluded.updated_at;
        )";

        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, dto.id);
        stmt.bindString(2, dto.name);
        stmt.bindString(3, dto.path);
        stmt.bindString(4, dto.content);
        
        if (dto.parentFolderId.empty()) {
            stmt.bindNull(5);
        } else {
            stmt.bindString(5, dto.parentFolderId);
        }
        
        stmt.bindInt64(6, dto.createdAt);
        stmt.bindInt64(7, dto.updatedAt);

        if (!stmt.execute()) {
            throw std::runtime_error("Failed to save note");
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

std::optional<plotter::dto::NoteDTO*> SqliteNoteDataSource::findById(const std::string& id) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = "SELECT id, name, path, content, parent_folder_id, created_at, updated_at FROM notes WHERE id = ?;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, id);

        std::optional<plotter::dto::NoteDTO*> result;
        if (stmt.step() == SQLITE_ROW) {
            auto* dto = new sqlite_dtos::SqliteNoteDTO();
            dto->id = stmt.getColumnString(0);
            dto->name = stmt.getColumnString(1);
            dto->path = stmt.getColumnString(2);
            dto->content = stmt.isColumnNull(3) ? "" : stmt.getColumnString(3);
            dto->parentFolderId = stmt.isColumnNull(4) ? "" : stmt.getColumnString(4);
            dto->createdAt = stmt.getColumnInt64(5);
            dto->updatedAt = stmt.getColumnInt64(6);
            
            result = dto;
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

std::vector<plotter::dto::NoteDTO*> SqliteNoteDataSource::findAll() {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = "SELECT id, name, path, content, parent_folder_id, created_at, updated_at FROM notes;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        std::vector<plotter::dto::NoteDTO*> notes;

        while (stmt.step() == SQLITE_ROW) {
            auto* dto = new sqlite_dtos::SqliteNoteDTO();
            dto->id = stmt.getColumnString(0);
            dto->name = stmt.getColumnString(1);
            dto->path = stmt.getColumnString(2);
            dto->content = stmt.isColumnNull(3) ? "" : stmt.getColumnString(3);
            dto->parentFolderId = stmt.isColumnNull(4) ? "" : stmt.getColumnString(4);
            dto->createdAt = stmt.getColumnInt64(5);
            dto->updatedAt = stmt.getColumnInt64(6);
            
            notes.push_back(dto);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return notes;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

std::vector<plotter::dto::NoteDTO*> SqliteNoteDataSource::findByParentFolderId(const std::string& parentFolderId) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = "SELECT id, name, path, content, parent_folder_id, created_at, updated_at FROM notes WHERE parent_folder_id = ?;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, parentFolderId);
        
        std::vector<plotter::dto::NoteDTO*> notes;

        while (stmt.step() == SQLITE_ROW) {
            auto* dto = new sqlite_dtos::SqliteNoteDTO();
            dto->id = stmt.getColumnString(0);
            dto->name = stmt.getColumnString(1);
            dto->path = stmt.getColumnString(2);
            dto->content = stmt.isColumnNull(3) ? "" : stmt.getColumnString(3);
            dto->parentFolderId = stmt.isColumnNull(4) ? "" : stmt.getColumnString(4);
            dto->createdAt = stmt.getColumnInt64(5);
            dto->updatedAt = stmt.getColumnInt64(6);
            
            notes.push_back(dto);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return notes;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

std::vector<plotter::dto::NoteDTO*> SqliteNoteDataSource::search(const std::string& searchTerm) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = "SELECT id, name, path, content, parent_folder_id, created_at, updated_at FROM notes WHERE name LIKE ? OR content LIKE ?;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        std::string pattern = "%" + searchTerm + "%";
        stmt.bindString(1, pattern);
        stmt.bindString(2, pattern);
        
        std::vector<plotter::dto::NoteDTO*> notes;

        while (stmt.step() == SQLITE_ROW) {
            auto* dto = new sqlite_dtos::SqliteNoteDTO();
            dto->id = stmt.getColumnString(0);
            dto->name = stmt.getColumnString(1);
            dto->path = stmt.getColumnString(2);
            dto->content = stmt.isColumnNull(3) ? "" : stmt.getColumnString(3);
            dto->parentFolderId = stmt.isColumnNull(4) ? "" : stmt.getColumnString(4);
            dto->createdAt = stmt.getColumnInt64(5);
            dto->updatedAt = stmt.getColumnInt64(6);
            
            notes.push_back(dto);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return notes;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

bool SqliteNoteDataSource::deleteById(const std::string& id) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = "DELETE FROM notes WHERE id = ?;";
        
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

bool SqliteNoteDataSource::update(const plotter::dto::NoteDTO& noteDTO) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const sqlite_dtos::SqliteNoteDTO& dto = dynamic_cast<const sqlite_dtos::SqliteNoteDTO&>(noteDTO);

        std::cout << "[DEBUG] SqliteNoteDataSource::update - checking if note exists: " << dto.id << std::endl;
        
        if (!exists(dto.id)) {
            std::cout << "[ERROR] SqliteNoteDataSource::update - note does not exist: " << dto.id << std::endl;
            return false;
        }

        std::cout << "[DEBUG] SqliteNoteDataSource::update - note exists, proceeding with UPDATE" << std::endl;

        const char* sql = R"(
            UPDATE notes 
            SET name = ?, path = ?, content = ?, parent_folder_id = ?, updated_at = ?
            WHERE id = ?;
        )";

        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, dto.name);
        stmt.bindString(2, dto.path);
        stmt.bindString(3, dto.content);
        
        std::cout << "[DEBUG] SqliteNoteDataSource::update - binding content with length: " << dto.content.length() << std::endl;
        
        if (dto.parentFolderId.empty()) {
            stmt.bindNull(4);
        } else {
            stmt.bindString(4, dto.parentFolderId);
        }
        
        stmt.bindInt64(5, dto.updatedAt);
        stmt.bindString(6, dto.id);

        int result = stmt.step();
        bool success = (result == SQLITE_DONE);
        
        std::cout << "[DEBUG] SqliteNoteDataSource::update - step result: " << result 
                  << " (SQLITE_DONE=" << SQLITE_DONE << ", SQLITE_ROW=" << SQLITE_ROW << ")" << std::endl;
        std::cout << "[DEBUG] SqliteNoteDataSource::update - success: " << success << std::endl;
        
        if (!success) {
            char* errMsg = (char*)sqlite3_errmsg(database->getHandle());
            std::cout << "[ERROR] SqliteNoteDataSource::update - SQLite error: " << errMsg << std::endl;
        }

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

bool SqliteNoteDataSource::exists(const std::string& id) {
    try {
        if (!isAvailable()) {
            return false;
        }

        const char* sql = "SELECT 1 FROM notes WHERE id = ? LIMIT 1;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, id);

        return stmt.step() == SQLITE_ROW;
    } catch (const std::exception&) {
        return false;
    }
}

size_t SqliteNoteDataSource::clear() {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        SqliteStatement countStmt(database->getHandle(), "SELECT COUNT(*) FROM notes;");
        size_t count = 0;
        if (countStmt.step() == SQLITE_ROW) {
            count = countStmt.getColumnInt(0);
        }

        const char* sql = "DELETE FROM notes;";
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

void SqliteNoteDataSource::updateMetrics(bool success, double responseTimeMs) {
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

long long SqliteNoteDataSource::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

} // namespace sqlite
} // namespace plotter
