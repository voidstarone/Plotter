#ifndef SQLITE_PROJECT_DATASOURCE_H
#define SQLITE_PROJECT_DATASOURCE_H

#include "plotter_repositories/ProjectDataSource.h"
#include "plotter_sqlite/SqliteDatabase.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
#include <memory>
#include <string>
#include <chrono>

namespace plotter {
namespace sqlite {

/**
 * @brief SQLite implementation of ProjectDataSource interface
 * 
 * Provides persistent storage for Project entities using SQLite database.
 * Uses DTOs for database representation and mappers for entity conversion.
 */
class SqliteProjectDataSource : public plotter::repositories::ProjectDataSource {
private:
    std::string name;
    int priority;
    std::shared_ptr<SqliteDatabase> database;
    plotter::repositories::DataSourceMetrics metrics;
    bool available;

    // Helper methods
    void updateMetrics(bool success, double responseTimeMs);
    sqlite_dtos::SqliteProjectDTO* rowToDTO(SqliteStatement& stmt);
    std::vector<std::string> getFolderIdsByProjectId(const std::string& projectId);

public:
    /**
     * @brief Construct a new Sqlite Project DataSource
     * 
     * @param name Name of this datasource
     * @param dbPath Path to the SQLite database file
     * @param priority Priority level (higher = more preferred)
     */
    SqliteProjectDataSource(const std::string& name, const std::string& dbPath, int priority = 100);

    // DataSource interface
    std::string getName() const override;
    std::string getType() const override;
    int getPriority() const override;
    bool isAvailable() const override;
    plotter::repositories::HealthCheckResult checkHealth() override;
    plotter::repositories::DataSourceMetrics getMetrics() const override;
    void connect() override;
    void disconnect() override;

    // ProjectDataSource interface
    std::string save(const plotter::dto::ProjectDTO& projectDTO) override;
    std::optional<plotter::dto::ProjectDTO*> findById(const std::string& id) override;
    std::vector<plotter::dto::ProjectDTO*> findAll() override;
    bool deleteById(const std::string& id) override;
    bool update(const plotter::dto::ProjectDTO& projectDTO) override;
    bool exists(const std::string& id) override;
    size_t clear() override;
};

} // namespace sqlite
} // namespace plotter

#endif // SQLITE_PROJECT_DATASOURCE_H
