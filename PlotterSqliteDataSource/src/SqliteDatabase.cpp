#include "plotter_sqlite/SqliteDatabase.h"
#include <iostream>

namespace plotter {
namespace sqlite {

SqliteDatabase::SqliteDatabase(const std::string& dbPath)
    : db(nullptr), dbPath(dbPath), connected(false) {}

SqliteDatabase::~SqliteDatabase() {
    disconnect();
}

void SqliteDatabase::connect() {
    if (connected) {
        return;
    }

    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        throw std::runtime_error("Failed to open database: " + error);
    }

    connected = true;

    // Enable foreign keys
    execute("PRAGMA foreign_keys = ON;");

    // Initialize schema if needed
    if (!tableExists("projects")) {
        initializeSchema();
    }
}

void SqliteDatabase::disconnect() {
    if (connected && db) {
        sqlite3_close(db);
        db = nullptr;
        connected = false;
    }
}

void SqliteDatabase::initializeSchema() {
    const char* schema = R"(
        -- Projects table
        CREATE TABLE IF NOT EXISTS projects (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            description TEXT,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL
        );

        -- Folders table with proper foreign keys
        CREATE TABLE IF NOT EXISTS folders (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            description TEXT,
            parent_project_id TEXT,
            parent_folder_id TEXT,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL,
            FOREIGN KEY (parent_project_id) REFERENCES projects(id) ON DELETE CASCADE,
            FOREIGN KEY (parent_folder_id) REFERENCES folders(id) ON DELETE CASCADE,
            -- Constraint: folder must have either a parent project OR parent folder, not both
            CHECK (
                (parent_project_id IS NOT NULL AND parent_folder_id IS NULL) OR
                (parent_project_id IS NULL AND parent_folder_id IS NOT NULL) OR
                (parent_project_id IS NULL AND parent_folder_id IS NULL)
            )
        );

        -- Notes table with proper foreign key
        CREATE TABLE IF NOT EXISTS notes (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            path TEXT NOT NULL,
            content TEXT,
            parent_folder_id TEXT,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL,
            FOREIGN KEY (parent_folder_id) REFERENCES folders(id) ON DELETE CASCADE
        );

        -- Indexes for common queries
        CREATE INDEX IF NOT EXISTS idx_folders_parent_project 
            ON folders(parent_project_id);
        
        CREATE INDEX IF NOT EXISTS idx_folders_parent_folder 
            ON folders(parent_folder_id);
        
        CREATE INDEX IF NOT EXISTS idx_notes_parent_folder 
            ON notes(parent_folder_id);
        
        CREATE INDEX IF NOT EXISTS idx_notes_name 
            ON notes(name);
    )";

    execute(schema);
}

void SqliteDatabase::execute(const std::string& sql) {
    char* errorMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
    
    if (rc != SQLITE_OK) {
        std::string error = errorMsg ? errorMsg : "Unknown error";
        sqlite3_free(errorMsg);
        throw std::runtime_error("SQL execution failed: " + error);
    }
}

void SqliteDatabase::beginTransaction() {
    execute("BEGIN TRANSACTION;");
}

void SqliteDatabase::commitTransaction() {
    execute("COMMIT;");
}

void SqliteDatabase::rollbackTransaction() {
    execute("ROLLBACK;");
}

bool SqliteDatabase::tableExists(const std::string& tableName) {
    const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, tableName.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    
    bool exists = (rc == SQLITE_ROW);
    sqlite3_finalize(stmt);
    
    return exists;
}

// SqliteStatement implementation

SqliteStatement::SqliteStatement(sqlite3* db, const std::string& sql)
    : stmt(nullptr), db(db) {
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
}

SqliteStatement::~SqliteStatement() {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}

void SqliteStatement::bindString(int index, const std::string& value) {
    int rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind string parameter");
    }
}

void SqliteStatement::bindInt(int index, int value) {
    int rc = sqlite3_bind_int(stmt, index, value);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind int parameter");
    }
}

void SqliteStatement::bindInt64(int index, long long value) {
    int rc = sqlite3_bind_int64(stmt, index, value);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind int64 parameter");
    }
}

void SqliteStatement::bindNull(int index) {
    int rc = sqlite3_bind_null(stmt, index);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind null parameter");
    }
}

bool SqliteStatement::execute() {
    int rc = sqlite3_step(stmt);
    return (rc == SQLITE_DONE);
}

int SqliteStatement::step() {
    return sqlite3_step(stmt);
}

void SqliteStatement::reset() {
    sqlite3_reset(stmt);
}

std::string SqliteStatement::getColumnString(int index) {
    const unsigned char* text = sqlite3_column_text(stmt, index);
    return text ? std::string(reinterpret_cast<const char*>(text)) : "";
}

int SqliteStatement::getColumnInt(int index) {
    return sqlite3_column_int(stmt, index);
}

long long SqliteStatement::getColumnInt64(int index) {
    return sqlite3_column_int64(stmt, index);
}

bool SqliteStatement::isColumnNull(int index) {
    return sqlite3_column_type(stmt, index) == SQLITE_NULL;
}

} // namespace sqlite
} // namespace plotter
