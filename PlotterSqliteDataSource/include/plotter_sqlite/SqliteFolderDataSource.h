#ifndef SQLITE_FOLDER_DATASOURCE_H
#define SQLITE_FOLDER_DATASOURCE_H

#include "plotter_repositories/FolderDataSource.h"
#include "plotter_sqlite/SqliteDatabase.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
#include <memory>
#include <string>
#include <chrono>

namespace plotter {
namespace sqlite {

/**
 * @brief SQLite implementation of FolderDataSource interface
 * 
 * Provides persistent storage for Folder entities using SQLite database.
 * Uses DTOs for database representation and mappers for entity conversion.
 */
class SqliteFolderDataSource : public plotter::repositories::FolderDataSource {
private:
    std::string name;
    int priority;
    std::shared_ptr<SqliteDatabase> database;
    plotter::repositories::DataSourceMetrics metrics;
    bool available;

    // Helper methods
    void updateMetrics(bool success, double responseTimeMs);
    sqlite_dtos::SqliteFolderDTO* rowToDTO(SqliteStatement& stmt);
    std::vector<std::string> getNoteIdsByFolderId(const std::string& folderId);
    std::vector<std::string> getSubfolderIdsByParentId(const std::string& parentId);

public:
    /**
     * @brief Construct a new Sqlite Folder DataSource
     * 
     * @param name Name of this datasource
     * @param dbPath Path to the SQLite database file
     * @param priority Priority level (higher = more preferred)
     */
    SqliteFolderDataSource(const std::string& name, const std::string& dbPath, int priority = 100);

    // DataSource interface
    std::string getName() const override;
    std::string getType() const override;
    int getPriority() const override;
    bool isAvailable() const override;
    plotter::repositories::HealthCheckResult checkHealth() override;
    plotter::repositories::DataSourceMetrics getMetrics() const override;
    void connect() override;
    void disconnect() override;

    // FolderDataSource interface
    std::string save(const plotter::dto::FolderDTO& folderDTO) override;
    std::optional<plotter::dto::FolderDTO*> findById(const std::string& id) override;
    std::vector<plotter::dto::FolderDTO*> findAll() override;
    std::vector<plotter::dto::FolderDTO*> findByProjectId(const std::string& projectId) override;
    std::vector<plotter::dto::FolderDTO*> findByParentFolderId(const std::string& parentFolderId) override;
    bool deleteById(const std::string& id) override;
    bool update(const plotter::dto::FolderDTO& folderDTO) override;
    bool exists(const std::string& id) override;
    size_t clear() override;
};

} // namespace sqlite
} // namespace plotter

#endif // SQLITE_FOLDER_DATASOURCE_H
