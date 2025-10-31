#include <iostream>
#include <cassert>
#include "plotter_sqlite_dtos/SqliteDTOs.h"
#include "BaseDTOs.h"

using namespace plotter::sqlite_dtos;
using namespace plotter::dto;

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
// SqliteProjectDTO Tests
// ============================================================================

TEST(test_sqlite_project_dto_creation) {
    SqliteProjectDTO dto;
    dto.id = "proj-1";
    dto.name = "Test Project";
    dto.description = "A test project";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    assert(dto.id == "proj-1");
    assert(dto.name == "Test Project");
    assert(dto.description == "A test project");
    assert(dto.createdAt == 1234567890);
    assert(dto.updatedAt == 1234567890);
}

TEST(test_sqlite_project_dto_inheritance) {
    SqliteProjectDTO* sqliteDto = new SqliteProjectDTO();
    sqliteDto->id = "proj-1";
    sqliteDto->name = "Test";
    
    // Can be treated as base DTO
    ProjectDTO* baseDto = sqliteDto;
    assert(baseDto != nullptr);
    
    // Can be dynamic_cast back
    SqliteProjectDTO* castedDto = dynamic_cast<SqliteProjectDTO*>(baseDto);
    assert(castedDto != nullptr);
    assert(castedDto->id == "proj-1");
    assert(castedDto->name == "Test");
    
    delete sqliteDto;
}

TEST(test_sqlite_project_dto_folder_ids) {
    SqliteProjectDTO dto;
    dto.id = "proj-1";
    dto.folderIds.push_back("folder-1");
    dto.folderIds.push_back("folder-2");
    dto.folderIds.push_back("folder-3");
    
    assert(dto.folderIds.size() == 3);
    assert(dto.folderIds[0] == "folder-1");
    assert(dto.folderIds[1] == "folder-2");
    assert(dto.folderIds[2] == "folder-3");
}

// ============================================================================
// SqliteFolderDTO Tests
// ============================================================================

TEST(test_sqlite_folder_dto_creation) {
    SqliteFolderDTO dto;
    dto.id = "folder-1";
    dto.name = "Documents";
    dto.description = "My documents";
    dto.parentProjectId = "proj-1";
    dto.parentFolderId = "";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    assert(dto.id == "folder-1");
    assert(dto.name == "Documents");
    assert(dto.description == "My documents");
    assert(dto.parentProjectId == "proj-1");
    assert(dto.parentFolderId == "");
}

TEST(test_sqlite_folder_dto_inheritance) {
    SqliteFolderDTO* sqliteDto = new SqliteFolderDTO();
    sqliteDto->id = "folder-1";
    
    FolderDTO* baseDto = sqliteDto;
    assert(baseDto != nullptr);
    
    SqliteFolderDTO* castedDto = dynamic_cast<SqliteFolderDTO*>(baseDto);
    assert(castedDto != nullptr);
    assert(castedDto->id == "folder-1");
    
    delete sqliteDto;
}

TEST(test_sqlite_folder_dto_children) {
    SqliteFolderDTO dto;
    dto.id = "folder-1";
    
    dto.noteIds.push_back("note-1");
    dto.noteIds.push_back("note-2");
    dto.subfolderIds.push_back("subfolder-1");
    
    assert(dto.noteIds.size() == 2);
    assert(dto.subfolderIds.size() == 1);
    assert(dto.noteIds[0] == "note-1");
    assert(dto.subfolderIds[0] == "subfolder-1");
}

// ============================================================================
// SqliteNoteDTO Tests
// ============================================================================

TEST(test_sqlite_note_dto_creation) {
    SqliteNoteDTO dto;
    dto.id = "note-1";
    dto.name = "Meeting Notes";
    dto.path = "/notes/meeting.md";
    dto.content = "Today we discussed...";
    dto.parentFolderId = "folder-1";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    assert(dto.id == "note-1");
    assert(dto.name == "Meeting Notes");
    assert(dto.path == "/notes/meeting.md");
    assert(dto.content == "Today we discussed...");
    assert(dto.parentFolderId == "folder-1");
}

TEST(test_sqlite_note_dto_inheritance) {
    SqliteNoteDTO* sqliteDto = new SqliteNoteDTO();
    sqliteDto->id = "note-1";
    sqliteDto->content = "Test content";
    
    NoteDTO* baseDto = sqliteDto;
    assert(baseDto != nullptr);
    
    SqliteNoteDTO* castedDto = dynamic_cast<SqliteNoteDTO*>(baseDto);
    assert(castedDto != nullptr);
    assert(castedDto->id == "note-1");
    assert(castedDto->content == "Test content");
    
    delete sqliteDto;
}

TEST(test_sqlite_note_dto_empty_content) {
    SqliteNoteDTO dto;
    dto.id = "note-1";
    dto.name = "Empty Note";
    dto.content = "";
    
    assert(dto.content.empty());
}

// ============================================================================
// SqliteDTOUtils Tests
// ============================================================================

TEST(test_get_current_timestamp) {
    long long timestamp1 = SqliteDTOUtils::getCurrentTimestamp();
    
    // Timestamp should be positive
    assert(timestamp1 > 0);
    
    // Wait a tiny bit and get another timestamp
    for (volatile int i = 0; i < 10000; i++);
    
    long long timestamp2 = SqliteDTOUtils::getCurrentTimestamp();
    
    // Second timestamp should be >= first
    assert(timestamp2 >= timestamp1);
}

// ============================================================================
// Polymorphism Tests
// ============================================================================

TEST(test_dto_polymorphism) {
    // Create concrete DTOs
    SqliteProjectDTO* projDto = new SqliteProjectDTO();
    SqliteFolderDTO* folderDto = new SqliteFolderDTO();
    SqliteNoteDTO* noteDto = new SqliteNoteDTO();
    
    projDto->id = "proj-1";
    folderDto->id = "folder-1";
    noteDto->id = "note-1";
    
    // Store as base pointers
    ProjectDTO* baseProj = projDto;
    FolderDTO* baseFolder = folderDto;
    NoteDTO* baseNote = noteDto;
    
    // Can cast back to concrete types
    SqliteProjectDTO* proj = dynamic_cast<SqliteProjectDTO*>(baseProj);
    SqliteFolderDTO* folder = dynamic_cast<SqliteFolderDTO*>(baseFolder);
    SqliteNoteDTO* note = dynamic_cast<SqliteNoteDTO*>(baseNote);
    
    assert(proj != nullptr && proj->id == "proj-1");
    assert(folder != nullptr && folder->id == "folder-1");
    assert(note != nullptr && note->id == "note-1");
    
    delete projDto;
    delete folderDto;
    delete noteDto;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "=== PlotterSqliteDTOs Test Suite ===" << std::endl << std::endl;
    
    // SqliteProjectDTO tests
    std::cout << "--- SqliteProjectDTO Tests ---" << std::endl;
    run_test_sqlite_project_dto_creation();
    run_test_sqlite_project_dto_inheritance();
    run_test_sqlite_project_dto_folder_ids();
    
    // SqliteFolderDTO tests
    std::cout << "\n--- SqliteFolderDTO Tests ---" << std::endl;
    run_test_sqlite_folder_dto_creation();
    run_test_sqlite_folder_dto_inheritance();
    run_test_sqlite_folder_dto_children();
    
    // SqliteNoteDTO tests
    std::cout << "\n--- SqliteNoteDTO Tests ---" << std::endl;
    run_test_sqlite_note_dto_creation();
    run_test_sqlite_note_dto_inheritance();
    run_test_sqlite_note_dto_empty_content();
    
    // Utils tests
    std::cout << "\n--- SqliteDTOUtils Tests ---" << std::endl;
    run_test_get_current_timestamp();
    
    // Polymorphism tests
    std::cout << "\n--- Polymorphism Tests ---" << std::endl;
    run_test_dto_polymorphism();
    
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
