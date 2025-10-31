#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H

#include <sqlite3.h>
#include <string>
#include <memory>
#include <stdexcept>

namespace plotter {
namespace sqlite {

/**
 * @brief RAII wrapper for SQLite database connection
 * 
 * Handles database initialization, schema creation, and connection management
 */
class SqliteDatabase {
private:
    sqlite3* db;
    std::string dbPath;
    bool connected;

public:
    /**
     * @brief Construct a new Sqlite Database object
     * 
     * @param dbPath Path to the SQLite database file
     */
    explicit SqliteDatabase(const std::string& dbPath);
    
    /**
     * @brief Destroy the Sqlite Database object and close connection
     */
    ~SqliteDatabase();

    // Prevent copying
    SqliteDatabase(const SqliteDatabase&) = delete;
    SqliteDatabase& operator=(const SqliteDatabase&) = delete;

    /**
     * @brief Open the database connection and create schema if needed
     */
    void connect();

    /**
     * @brief Close the database connection
     */
    void disconnect();

    /**
     * @brief Check if the database is connected
     */
    bool isConnected() const { return connected; }

    /**
     * @brief Get the raw SQLite database handle
     */
    sqlite3* getHandle() const { return db; }

    /**
     * @brief Initialize the database schema
     */
    void initializeSchema();

    /**
     * @brief Execute a SQL statement without returning results
     * 
     * @param sql The SQL statement to execute
     * @throws std::runtime_error if execution fails
     */
    void execute(const std::string& sql);

    /**
     * @brief Begin a transaction
     */
    void beginTransaction();

    /**
     * @brief Commit a transaction
     */
    void commitTransaction();

    /**
     * @brief Rollback a transaction
     */
    void rollbackTransaction();

    /**
     * @brief Check if a table exists
     * 
     * @param tableName The name of the table
     * @return true if the table exists
     */
    bool tableExists(const std::string& tableName);
};

/**
 * @brief RAII wrapper for SQLite prepared statements
 */
class SqliteStatement {
private:
    sqlite3_stmt* stmt;
    sqlite3* db;

public:
    SqliteStatement(sqlite3* db, const std::string& sql);
    ~SqliteStatement();

    // Prevent copying
    SqliteStatement(const SqliteStatement&) = delete;
    SqliteStatement& operator=(const SqliteStatement&) = delete;

    /**
     * @brief Get the raw statement handle
     */
    sqlite3_stmt* getHandle() const { return stmt; }

    /**
     * @brief Bind a string parameter
     */
    void bindString(int index, const std::string& value);

    /**
     * @brief Bind an integer parameter
     */
    void bindInt(int index, int value);

    /**
     * @brief Bind a long long parameter
     */
    void bindInt64(int index, long long value);

    /**
     * @brief Bind a NULL parameter
     */
    void bindNull(int index);

    /**
     * @brief Execute the statement (for INSERT/UPDATE/DELETE)
     * 
     * @return true if successful
     */
    bool execute();

    /**
     * @brief Step through results (for SELECT)
     * 
     * @return SQLITE_ROW if a row is available, SQLITE_DONE if finished
     */
    int step();

    /**
     * @brief Reset the statement for reuse
     */
    void reset();

    /**
     * @brief Get string column value
     */
    std::string getColumnString(int index);

    /**
     * @brief Get integer column value
     */
    int getColumnInt(int index);

    /**
     * @brief Get long long column value
     */
    long long getColumnInt64(int index);

    /**
     * @brief Check if column is NULL
     */
    bool isColumnNull(int index);
};

} // namespace sqlite
} // namespace plotter

#endif // SQLITE_DATABASE_H
