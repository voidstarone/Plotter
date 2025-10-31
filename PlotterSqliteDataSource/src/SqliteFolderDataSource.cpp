#include "plotter_sqlite/SqliteFolderDataSource.h"

namespace plotter {
namespace sqlite {

SqliteFolderDataSource::SqliteFolderDataSource(const std::string& name, const std::string& dbPath, int priority)
    : name(name), priority(priority), database(std::make_shared<SqliteDatabase>(dbPath)), available(false) {}

std::string SqliteFolderDataSource::getName() const {
    return name;
}

std::string SqliteFolderDataSource::getType() const {
    return "SQLite";
}

int SqliteFolderDataSource::getPriority() const {
    return priority;
}

bool SqliteFolderDataSource::isAvailable() const {
    return available && database && database->isConnected();
}

plotter::repositories::HealthCheckResult SqliteFolderDataSource::checkHealth() {
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
        SqliteStatement stmt(database->getHandle(), "SELECT COUNT(*) FROM folders;");
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

plotter::repositories::DataSourceMetrics SqliteFolderDataSource::getMetrics() const {
    return metrics;
}

void SqliteFolderDataSource::connect() {
    try {
        database->connect();
        available = true;
    } catch (const std::exception& e) {
        available = false;
        throw;
    }
}

void SqliteFolderDataSource::disconnect() {
    database->disconnect();
    available = false;
}

std::string SqliteFolderDataSource::save(const plotter::dto::FolderDTO& folderDTO) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const sqlite_dtos::SqliteFolderDTO& dto = dynamic_cast<const sqlite_dtos::SqliteFolderDTO&>(folderDTO);

        const char* sql = R"(
            INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
            ON CONFLICT(id) DO UPDATE SET
                name = excluded.name,
                description = excluded.description,
                parent_project_id = excluded.parent_project_id,
                parent_folder_id = excluded.parent_folder_id,
                updated_at = excluded.updated_at;
        )";

        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, dto.id);
        stmt.bindString(2, dto.name);
        stmt.bindString(3, dto.description);
        
        if (dto.parentProjectId.empty()) {
            stmt.bindNull(4);
        } else {
            stmt.bindString(4, dto.parentProjectId);
        }
        
        if (dto.parentFolderId.empty()) {
            stmt.bindNull(5);
        } else {
            stmt.bindString(5, dto.parentFolderId);
        }
        
        stmt.bindInt64(6, dto.createdAt);
        stmt.bindInt64(7, dto.updatedAt);

        if (!stmt.execute()) {
            throw std::runtime_error("Failed to save folder");
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

std::optional<plotter::dto::FolderDTO*> SqliteFolderDataSource::findById(const std::string& id) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = R"(
            SELECT id, name, description, parent_project_id, parent_folder_id, created_at, updated_at
            FROM folders WHERE id = ?;
        )";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, id);

        std::optional<plotter::dto::FolderDTO*> result;
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

std::vector<plotter::dto::FolderDTO*> SqliteFolderDataSource::findAll() {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = R"(
            SELECT id, name, description, parent_project_id, parent_folder_id, created_at, updated_at
            FROM folders;
        )";
        
        SqliteStatement stmt(database->getHandle(), sql);
        std::vector<plotter::dto::FolderDTO*> folders;

        while (stmt.step() == SQLITE_ROW) {
            folders.push_back(rowToDTO(stmt));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return folders;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

std::vector<plotter::dto::FolderDTO*> SqliteFolderDataSource::findByProjectId(const std::string& projectId) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = R"(
            SELECT id, name, description, parent_project_id, parent_folder_id, created_at, updated_at
            FROM folders WHERE parent_project_id = ?;
        )";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, projectId);
        
        std::vector<plotter::dto::FolderDTO*> folders;

        while (stmt.step() == SQLITE_ROW) {
            folders.push_back(rowToDTO(stmt));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return folders;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

std::vector<plotter::dto::FolderDTO*> SqliteFolderDataSource::findByParentFolderId(const std::string& parentFolderId) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = R"(
            SELECT id, name, description, parent_project_id, parent_folder_id, created_at, updated_at
            FROM folders WHERE parent_folder_id = ?;
        )";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, parentFolderId);
        
        std::vector<plotter::dto::FolderDTO*> folders;

        while (stmt.step() == SQLITE_ROW) {
            folders.push_back(rowToDTO(stmt));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(true, elapsed.count());

        return folders;
    } catch (const std::exception& e) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        updateMetrics(false, elapsed.count());
        throw;
    }
}

bool SqliteFolderDataSource::deleteById(const std::string& id) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const char* sql = "DELETE FROM folders WHERE id = ?;";
        
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

bool SqliteFolderDataSource::update(const plotter::dto::FolderDTO& folderDTO) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        const sqlite_dtos::SqliteFolderDTO& dto = dynamic_cast<const sqlite_dtos::SqliteFolderDTO&>(folderDTO);

        if (!exists(dto.id)) {
            return false;
        }

        const char* sql = R"(
            UPDATE folders 
            SET name = ?, description = ?, parent_project_id = ?, parent_folder_id = ?, updated_at = ?
            WHERE id = ?;
        )";

        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, dto.name);
        stmt.bindString(2, dto.description);
        
        if (dto.parentProjectId.empty()) {
            stmt.bindNull(3);
        } else {
            stmt.bindString(3, dto.parentProjectId);
        }
        
        if (dto.parentFolderId.empty()) {
            stmt.bindNull(4);
        } else {
            stmt.bindString(4, dto.parentFolderId);
        }
        
        stmt.bindInt64(5, dto.updatedAt);
        stmt.bindString(6, dto.id);

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

bool SqliteFolderDataSource::exists(const std::string& id) {
    try {
        if (!isAvailable()) {
            return false;
        }

        const char* sql = "SELECT 1 FROM folders WHERE id = ? LIMIT 1;";
        
        SqliteStatement stmt(database->getHandle(), sql);
        stmt.bindString(1, id);

        return stmt.step() == SQLITE_ROW;
    } catch (const std::exception&) {
        return false;
    }
}

size_t SqliteFolderDataSource::clear() {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (!isAvailable()) {
            throw std::runtime_error("Database is not available");
        }

        SqliteStatement countStmt(database->getHandle(), "SELECT COUNT(*) FROM folders;");
        size_t count = 0;
        if (countStmt.step() == SQLITE_ROW) {
            count = countStmt.getColumnInt(0);
        }

        const char* sql = "DELETE FROM folders;";
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

void SqliteFolderDataSource::updateMetrics(bool success, double responseTimeMs) {
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

sqlite_dtos::SqliteFolderDTO* SqliteFolderDataSource::rowToDTO(SqliteStatement& stmt) {
    sqlite_dtos::SqliteFolderDTO* dto = new sqlite_dtos::SqliteFolderDTO();
    dto->id = stmt.getColumnString(0);
    dto->name = stmt.getColumnString(1);
    dto->description = stmt.getColumnString(2);
    dto->parentProjectId = stmt.isColumnNull(3) ? "" : stmt.getColumnString(3);
    dto->parentFolderId = stmt.isColumnNull(4) ? "" : stmt.getColumnString(4);
    dto->createdAt = stmt.getColumnInt64(5);
    dto->updatedAt = stmt.getColumnInt64(6);
    return dto;
}

std::vector<std::string> SqliteFolderDataSource::getNoteIdsByFolderId(const std::string& folderId) {
    std::vector<std::string> noteIds;
    
    const char* sql = "SELECT id FROM notes WHERE parent_folder_id = ?;";
    SqliteStatement stmt(database->getHandle(), sql);
    stmt.bindString(1, folderId);
    
    while (stmt.step() == SQLITE_ROW) {
        noteIds.push_back(stmt.getColumnString(0));
    }
    
    return noteIds;
}

std::vector<std::string> SqliteFolderDataSource::getSubfolderIdsByParentId(const std::string& parentId) {
    std::vector<std::string> subfolderIds;
    
    const char* sql = "SELECT id FROM folders WHERE parent_folder_id = ?;";
    SqliteStatement stmt(database->getHandle(), sql);
    stmt.bindString(1, parentId);
    
    while (stmt.step() == SQLITE_ROW) {
        subfolderIds.push_back(stmt.getColumnString(0));
    }
    
    return subfolderIds;
}

} // namespace sqlite
} // namespace plotter
