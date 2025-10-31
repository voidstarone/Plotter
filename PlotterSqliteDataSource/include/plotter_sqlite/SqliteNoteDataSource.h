#ifndef SQLITE_NOTE_DATASOURCE_H
#define SQLITE_NOTE_DATASOURCE_H

#include "plotter_repositories/NoteDataSource.h"
#include "plotter_sqlite/SqliteDatabase.h"
#include <memory>
#include <string>
#include <chrono>

namespace plotter {
namespace sqlite {

/**
 * @brief SQLite implementation of NoteDataSource interface
 * 
 * Provides persistent storage for Note metadata using SQLite database.
 * Note: This only stores note metadata (id, name, path, parent folder).
 * Actual note content is stored via the NoteStorage interface.
 */
class SqliteNoteDataSource : public plotter::repositories::NoteDataSource {
private:
    std::string name;
    int priority;
    std::shared_ptr<SqliteDatabase> database;
    plotter::repositories::DataSourceMetrics metrics;
    bool available;

    // Helper methods
    void updateMetrics(bool success, double responseTimeMs);
    long long getCurrentTimestamp();

public:
    /**
     * @brief Construct a new Sqlite Note DataSource
     * 
     * @param name Name of this datasource
     * @param dbPath Path to the SQLite database file
     * @param priority Priority level (higher = more preferred)
     */
    SqliteNoteDataSource(const std::string& name, const std::string& dbPath, int priority = 100);

    // DataSource interface
    std::string getName() const override;
    std::string getType() const override;
    int getPriority() const override;
    bool isAvailable() const override;
    plotter::repositories::HealthCheckResult checkHealth() override;
    plotter::repositories::DataSourceMetrics getMetrics() const override;
    void connect() override;
    void disconnect() override;

    // NoteDataSource interface - works with DTOs
    std::string save(const plotter::dto::NoteDTO& noteDTO) override;
    std::optional<plotter::dto::NoteDTO*> findById(const std::string& id) override;
    std::vector<plotter::dto::NoteDTO*> findAll() override;
    std::vector<plotter::dto::NoteDTO*> findByParentFolderId(const std::string& parentFolderId) override;
    std::vector<plotter::dto::NoteDTO*> search(const std::string& searchTerm) override;
    bool deleteById(const std::string& id) override;
    bool update(const plotter::dto::NoteDTO& noteDTO) override;
    bool exists(const std::string& id) override;
    size_t clear() override;
};

} // namespace sqlite
} // namespace plotter

#endif // SQLITE_NOTE_DATASOURCE_H
