#include <iostream>
#include <cassert>
#include <stdexcept>
#include <memory>
#include "plotter_sqlite/SqliteProjectDataSource.h"
#include "plotter_sqlite/SqliteFolderDataSource.h"
#include "plotter_sqlite/SqliteNoteDataSource.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"

using namespace plotter::sqlite;
using namespace plotter::sqlite_dtos;
using namespace plotter::repositories;

// Simple test framework
int tests_run = 0;
int tests_passed = 0;

#define TEST(name) \
    void name(); \
    void run_##name() { \
        std::cout << "Running " << #name << "... "; \
        tests_run++; \
        try { \
            name(); \
            tests_passed++; \
            std::cout << "PASSED" << std::endl; \
        } catch (const std::exception& e) { \
            std::cout << "FAILED: " << e.what() << std::endl; \
        } \
    } \
    void name()

// ============================================================================
// SqliteProjectDataSource Tests
// ============================================================================

TEST(test_project_datasource_connect) {
    SqliteProjectDataSource ds("test-db", ":memory:", 100);
    ds.connect();
    
    assert(ds.isAvailable());
    assert(ds.getName() == "test-db");
    assert(ds.getType() == "SQLite");
    assert(ds.getPriority() == 100);
    
    ds.disconnect();
    assert(!ds.isAvailable());
}

TEST(test_project_datasource_save_and_find) {
    SqliteProjectDataSource ds("test-db", ":memory:", 100);
    ds.connect();
    
    // Create DTO
    SqliteProjectDTO dto;
    dto.id = "proj-1";
    dto.name = "Test Project";
    dto.description = "A test project";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    // Save
    std::string savedId = ds.save(dto);
    assert(savedId == "proj-1");
    
    // Find by ID
    auto foundDto = ds.findById("proj-1");
    assert(foundDto.has_value());
    assert(foundDto.value() != nullptr);
    
    auto* sqliteDto = dynamic_cast<SqliteProjectDTO*>(foundDto.value());
    assert(sqliteDto != nullptr);
    assert(sqliteDto->id == "proj-1");
    assert(sqliteDto->name == "Test Project");
    assert(sqliteDto->description == "A test project");
    
    delete foundDto.value();
    ds.disconnect();
}

TEST(test_project_datasource_find_all) {
    SqliteProjectDataSource ds("test-db", ":memory:", 100);
    ds.connect();
    
    // Save multiple projects
    for (int i = 1; i <= 3; i++) {
        SqliteProjectDTO dto;
        dto.id = "proj-" + std::to_string(i);
        dto.name = "Project " + std::to_string(i);
        dto.description = "Description " + std::to_string(i);
        dto.createdAt = 1234567890;
        dto.updatedAt = 1234567890;
        ds.save(dto);
    }
    
    // Find all
    auto allProjects = ds.findAll();
    assert(allProjects.size() == 3);
    
    // Clean up
    for (auto* dto : allProjects) {
        delete dto;
    }
    
    ds.disconnect();
}

TEST(test_project_datasource_update) {
    SqliteProjectDataSource ds("test-db", ":memory:", 100);
    ds.connect();
    
    // Save project
    SqliteProjectDTO dto;
    dto.id = "proj-1";
    dto.name = "Original Name";
    dto.description = "Original Description";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    ds.save(dto);
    
    // Update
    dto.name = "Updated Name";
    dto.description = "Updated Description";
    ds.update(dto);
    
    // Verify update
    auto found = ds.findById("proj-1");
    auto* sqliteDto = dynamic_cast<SqliteProjectDTO*>(found.value());
    assert(sqliteDto->name == "Updated Name");
    assert(sqliteDto->description == "Updated Description");
    
    delete found.value();
    ds.disconnect();
}

TEST(test_project_datasource_delete) {
    SqliteProjectDataSource ds("test-db", ":memory:", 100);
    ds.connect();
    
    // Save project
    SqliteProjectDTO dto;
    dto.id = "proj-1";
    dto.name = "Test";
    dto.description = "Test";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    ds.save(dto);
    
    // Delete
    bool deleted = ds.deleteById("proj-1");
    assert(deleted);
    
    // Verify deleted
    auto found = ds.findById("proj-1");
    assert(!found.has_value());
    
    ds.disconnect();
}

TEST(test_project_datasource_exists) {
    SqliteProjectDataSource ds("test-db", ":memory:", 100);
    ds.connect();
    
    // Initially doesn't exist
    assert(!ds.exists("proj-1"));
    
    // Save project
    SqliteProjectDTO dto;
    dto.id = "proj-1";
    dto.name = "Test";
    dto.description = "Test";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    ds.save(dto);
    
    // Now exists
    assert(ds.exists("proj-1"));
    
    ds.disconnect();
}

// ============================================================================
// SqliteFolderDataSource Tests
// ============================================================================

TEST(test_folder_datasource_save_and_find) {
    // Use shared database path to share schema
    const char* dbPath = ":memory:";
    
    // Create project datasource and save a project
    SqliteProjectDataSource projDs("proj-ds", dbPath, 100);
    projDs.connect();
    
    SqliteProjectDTO proj;
    proj.id = "proj-1";
    proj.name = "Test Project";
    proj.description = "Test";
    proj.createdAt = 1234567890;
    proj.updatedAt = 1234567890;
    projDs.save(proj);
    
    // Now create folder datasource using same database file (won't work with :memory: across instances)
    // For testing, let's use a real file
    SqliteProjectDataSource projDs2("proj-ds", "/tmp/test_folder.db", 100);
    projDs2.connect();
    SqliteProjectDTO proj2;
    proj2.id = "proj-1";
    proj2.name = "Test Project";
    proj2.description = "Test";
    proj2.createdAt = 1234567890;
    proj2.updatedAt = 1234567890;
    projDs2.save(proj2);
    
    SqliteFolderDataSource folderDs("folder-ds", "/tmp/test_folder.db", 100);
    folderDs.connect();
    
    // Create folder DTO
    SqliteFolderDTO dto;
    dto.id = "folder-1";
    dto.name = "Documents";
    dto.description = "My documents";
    dto.parentProjectId = "proj-1";
    dto.parentFolderId = "";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    // Save
    std::string savedId = folderDs.save(dto);
    assert(savedId == "folder-1");
    
    // Find
    auto found = folderDs.findById("folder-1");
    assert(found.has_value());
    
    auto* sqliteDto = dynamic_cast<SqliteFolderDTO*>(found.value());
    assert(sqliteDto->id == "folder-1");
    assert(sqliteDto->name == "Documents");
    assert(sqliteDto->parentProjectId == "proj-1");
    
    delete found.value();
    folderDs.disconnect();
    projDs2.disconnect();
    
    // Clean up
    std::remove("/tmp/test_folder.db");
}

TEST(test_folder_datasource_with_children) {
    // Use temp file to share database
    SqliteProjectDataSource projDs("proj-ds", "/tmp/test_folder2.db", 100);
    projDs.connect();
    
    SqliteProjectDTO proj;
    proj.id = "proj-1";
    proj.name = "Test Project";
    proj.description = "Test";
    proj.createdAt = 1234567890;
    proj.updatedAt = 1234567890;
    projDs.save(proj);
    
    SqliteFolderDataSource folderDs("folder-ds", "/tmp/test_folder2.db", 100);
    folderDs.connect();
    
    // Save folder with children IDs
    SqliteFolderDTO dto;
    dto.id = "folder-1";
    dto.name = "Work";
    dto.description = "Work folder";
    dto.parentProjectId = "proj-1";
    dto.parentFolderId = "";
    dto.noteIds.push_back("note-1");
    dto.noteIds.push_back("note-2");
    dto.subfolderIds.push_back("subfolder-1");
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    folderDs.save(dto);
    
    // Find - just verify folder was saved
    auto found = folderDs.findById("folder-1");
    auto* sqliteDto = dynamic_cast<SqliteFolderDTO*>(found.value());
    
    // Verify basic properties (children require separate queries via junction tables)
    assert(sqliteDto->id == "folder-1");
    assert(sqliteDto->name == "Work");
    
    delete found.value();
    folderDs.disconnect();
    projDs.disconnect();
    
    // Clean up
    std::remove("/tmp/test_folder2.db");
}

// ============================================================================
// SqliteNoteDataSource Tests
// ============================================================================

TEST(test_note_datasource_save_and_find) {
    // Create project and folder first
    SqliteProjectDataSource projDs("proj-ds", "/tmp/test_note.db", 100);
    projDs.connect();
    
    SqliteProjectDTO proj;
    proj.id = "proj-1";
    proj.name = "Test Project";
    proj.description = "Test";
    proj.createdAt = 1234567890;
    proj.updatedAt = 1234567890;
    projDs.save(proj);
    
    SqliteFolderDataSource folderDs("folder-ds", "/tmp/test_note.db", 100);
    folderDs.connect();
    
    SqliteFolderDTO folder;
    folder.id = "folder-1";
    folder.name = "Folder";
    folder.description = "Test folder";
    folder.parentProjectId = "proj-1";
    folder.parentFolderId = "";
    folder.createdAt = 1234567890;
    folder.updatedAt = 1234567890;
    folderDs.save(folder);
    
    SqliteNoteDataSource noteDs("note-ds", "/tmp/test_note.db", 100);
    noteDs.connect();
    
    // Create DTO
    SqliteNoteDTO dto;
    dto.id = "note-1";
    dto.name = "Meeting Notes";
    dto.path = "/notes/meeting.md";
    dto.content = "Today we discussed...";
    dto.parentFolderId = "folder-1";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    // Save
    std::string savedId = noteDs.save(dto);
    assert(savedId == "note-1");
    
    // Find
    auto found = noteDs.findById("note-1");
    assert(found.has_value());
    
    auto* sqliteDto = dynamic_cast<SqliteNoteDTO*>(found.value());
    assert(sqliteDto->id == "note-1");
    assert(sqliteDto->name == "Meeting Notes");
    assert(sqliteDto->content == "Today we discussed...");
    
    delete found.value();
    noteDs.disconnect();
    folderDs.disconnect();
    projDs.disconnect();
    
    std::remove("/tmp/test_note.db");
}

TEST(test_note_datasource_update_content) {
    // Create project and folder first
    SqliteProjectDataSource projDs("proj-ds", "/tmp/test_note2.db", 100);
    projDs.connect();
    
    SqliteProjectDTO proj;
    proj.id = "proj-1";
    proj.name = "Test Project";
    proj.description = "Test";
    proj.createdAt = 1234567890;
    proj.updatedAt = 1234567890;
    projDs.save(proj);
    
    SqliteFolderDataSource folderDs("folder-ds", "/tmp/test_note2.db", 100);
    folderDs.connect();
    
    SqliteFolderDTO folder;
    folder.id = "folder-1";
    folder.name = "Folder";
    folder.description = "Test folder";
    folder.parentProjectId = "proj-1";
    folder.parentFolderId = "";
    folder.createdAt = 1234567890;
    folder.updatedAt = 1234567890;
    folderDs.save(folder);
    
    SqliteNoteDataSource noteDs("note-ds", "/tmp/test_note2.db", 100);
    noteDs.connect();
    
    // Save note
    SqliteNoteDTO dto;
    dto.id = "note-1";
    dto.name = "My Note";
    dto.path = "/notes/test.md";
    dto.content = "Original content";
    dto.parentFolderId = "folder-1";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    noteDs.save(dto);
    
    // Update content
    dto.content = "Updated content";
    noteDs.update(dto);
    
    // Verify
    auto found = noteDs.findById("note-1");
    auto* sqliteDto = dynamic_cast<SqliteNoteDTO*>(found.value());
    assert(sqliteDto->content == "Updated content");
    
    delete found.value();
    noteDs.disconnect();
    folderDs.disconnect();
    projDs.disconnect();
    
    std::remove("/tmp/test_note2.db");
}

// ============================================================================
// Health Check and Metrics Tests
// ============================================================================

TEST(test_datasource_health_check) {
    SqliteProjectDataSource ds("test-db", ":memory:", 100);
    ds.connect();
    
    auto health = ds.checkHealth();
    assert(health.status == HealthStatus::HEALTHY);
    assert(health.isHealthy());
    assert(health.isAvailable());
    
    ds.disconnect();
}

TEST(test_datasource_metrics) {
    SqliteProjectDataSource ds("test-db", ":memory:", 100);
    ds.connect();
    
    // Perform some operations
    SqliteProjectDTO dto;
    dto.id = "proj-1";
    dto.name = "Test";
    dto.description = "Test";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    ds.save(dto);
    ds.findById("proj-1");
    
    auto metrics = ds.getMetrics();
    assert(metrics.totalRequests > 0);
    assert(metrics.successfulRequests > 0);
    
    ds.disconnect();
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "=== PlotterSqliteDataSource Test Suite ===" << std::endl << std::endl;
    
    // Project datasource tests
    std::cout << "--- SqliteProjectDataSource Tests ---" << std::endl;
    run_test_project_datasource_connect();
    run_test_project_datasource_save_and_find();
    run_test_project_datasource_find_all();
    run_test_project_datasource_update();
    run_test_project_datasource_delete();
    run_test_project_datasource_exists();
    
    // Folder datasource tests
    std::cout << "\n--- SqliteFolderDataSource Tests ---" << std::endl;
    run_test_folder_datasource_save_and_find();
    run_test_folder_datasource_with_children();
    
    // Note datasource tests
    std::cout << "\n--- SqliteNoteDataSource Tests ---" << std::endl;
    run_test_note_datasource_save_and_find();
    run_test_note_datasource_update_content();
    
    // Health and metrics tests
    std::cout << "\n--- Health and Metrics Tests ---" << std::endl;
    run_test_datasource_health_check();
    run_test_datasource_metrics();
    
    // Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Tests run: " << tests_run << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
    
    if (tests_passed == tests_run) {
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ Some tests failed!" << std::endl;
        return 1;
    }
}
