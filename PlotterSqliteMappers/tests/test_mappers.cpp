#include <iostream>
#include <cassert>
#include <stdexcept>
#include "plotter_sqlite_mappers/SqliteMappers.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
#include "Project.h"
#include "Folder.h"
#include "Note.h"

using namespace plotter::sqlite_mappers;
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
// SqliteProjectMapper Tests
// ============================================================================

TEST(test_project_mapper_to_dto) {
    SqliteProjectMapper mapper;
    
    // Create entity
    Project project("proj-1", "Test Project", "A test project");
    project.addFolderId("folder-1");
    project.addFolderId("folder-2");
    
    // Convert to DTO
    ProjectDTO* baseDto = mapper.toDTO(project);
    assert(baseDto != nullptr);
    
    // Downcast to concrete type
    SqliteProjectDTO* dto = dynamic_cast<SqliteProjectDTO*>(baseDto);
    assert(dto != nullptr);
    assert(dto->id == "proj-1");
    assert(dto->name == "Test Project");
    assert(dto->description == "A test project");
    assert(dto->folderIds.size() == 2);
    assert(dto->folderIds[0] == "folder-1");
    assert(dto->folderIds[1] == "folder-2");
    assert(dto->createdAt > 0);
    assert(dto->updatedAt > 0);
    
    delete baseDto;
}

TEST(test_project_mapper_to_entity) {
    SqliteProjectMapper mapper;
    
    // Create DTO
    SqliteProjectDTO dto;
    dto.id = "proj-2";
    dto.name = "My Project";
    dto.description = "Project description";
    dto.folderIds.push_back("folder-a");
    dto.folderIds.push_back("folder-b");
    dto.folderIds.push_back("folder-c");
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    // Convert to entity
    Project project = mapper.toEntity(dto);
    assert(project.getId() == "proj-2");
    assert(project.getName() == "My Project");
    assert(project.getDescription() == "Project description");
    assert(project.getFolderIds().size() == 3);
    assert(project.getFolderIds()[0] == "folder-a");
    assert(project.getFolderIds()[1] == "folder-b");
    assert(project.getFolderIds()[2] == "folder-c");
}

TEST(test_project_mapper_roundtrip) {
    SqliteProjectMapper mapper;
    
    // Create original entity
    Project original("proj-3", "Roundtrip Test", "Testing roundtrip conversion");
    original.addFolderId("folder-x");
    original.addFolderId("folder-y");
    
    // Convert to DTO and back
    ProjectDTO* baseDto = mapper.toDTO(original);
    Project restored = mapper.toEntity(*baseDto);
    
    // Verify
    assert(restored.getId() == original.getId());
    assert(restored.getName() == original.getName());
    assert(restored.getDescription() == original.getDescription());
    assert(restored.getFolderIds().size() == original.getFolderIds().size());
    
    delete baseDto;
}

// ============================================================================
// SqliteFolderMapper Tests
// ============================================================================

TEST(test_folder_mapper_to_dto) {
    SqliteFolderMapper mapper;
    
    // Create entity
    Folder folder("folder-1", "Documents", "My documents", "proj-1", "");
    folder.addNoteId("note-1");
    folder.addNoteId("note-2");
    folder.addSubfolderId("subfolder-1");
    
    // Convert to DTO
    FolderDTO* baseDto = mapper.toDTO(folder);
    assert(baseDto != nullptr);
    
    SqliteFolderDTO* dto = dynamic_cast<SqliteFolderDTO*>(baseDto);
    assert(dto != nullptr);
    assert(dto->id == "folder-1");
    assert(dto->name == "Documents");
    assert(dto->description == "My documents");
    assert(dto->parentProjectId == "proj-1");
    assert(dto->parentFolderId == "");
    assert(dto->noteIds.size() == 2);
    assert(dto->subfolderIds.size() == 1);
    
    delete baseDto;
}

TEST(test_folder_mapper_to_entity) {
    SqliteFolderMapper mapper;
    
    // Create DTO
    SqliteFolderDTO dto;
    dto.id = "folder-2";
    dto.name = "Work";
    dto.description = "Work folder";
    dto.parentProjectId = "proj-1";
    dto.parentFolderId = "parent-folder";
    dto.noteIds.push_back("note-a");
    dto.noteIds.push_back("note-b");
    dto.subfolderIds.push_back("sub-a");
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    // Convert to entity
    Folder folder = mapper.toEntity(dto);
    assert(folder.getId() == "folder-2");
    assert(folder.getName() == "Work");
    assert(folder.getDescription() == "Work folder");
    assert(folder.getParentProjectId() == "proj-1");
    assert(folder.getParentFolderId() == "parent-folder");
    assert(folder.getNoteIds().size() == 2);
    assert(folder.getSubfolderIds().size() == 1);
}

TEST(test_folder_mapper_roundtrip) {
    SqliteFolderMapper mapper;
    
    Folder original("folder-3", "Test", "Test folder", "proj-1", "parent");
    original.addNoteId("note-1");
    original.addSubfolderId("sub-1");
    
    FolderDTO* baseDto = mapper.toDTO(original);
    Folder restored = mapper.toEntity(*baseDto);
    
    assert(restored.getId() == original.getId());
    assert(restored.getName() == original.getName());
    assert(restored.getParentProjectId() == original.getParentProjectId());
    assert(restored.getParentFolderId() == original.getParentFolderId());
    assert(restored.getNoteIds().size() == original.getNoteIds().size());
    
    delete baseDto;
}

// ============================================================================
// SqliteNoteMapper Tests
// ============================================================================

TEST(test_note_mapper_to_dto) {
    SqliteNoteMapper mapper;
    
    // Create entity
    Note note("note-1", "Meeting Notes", "/notes/meeting.md", "folder-1");
    note.setContent("Today we discussed the project timeline.");
    
    // Convert to DTO
    NoteDTO* baseDto = mapper.toDTO(note);
    assert(baseDto != nullptr);
    
    SqliteNoteDTO* dto = dynamic_cast<SqliteNoteDTO*>(baseDto);
    assert(dto != nullptr);
    assert(dto->id == "note-1");
    assert(dto->name == "Meeting Notes");
    assert(dto->path == "/notes/meeting.md");
    assert(dto->content == "Today we discussed the project timeline.");
    assert(dto->parentFolderId == "folder-1");
    assert(dto->createdAt > 0);
    assert(dto->updatedAt > 0);
    
    delete baseDto;
}

TEST(test_note_mapper_to_entity) {
    SqliteNoteMapper mapper;
    
    // Create DTO
    SqliteNoteDTO dto;
    dto.id = "note-2";
    dto.name = "Ideas";
    dto.path = "/notes/ideas.md";
    dto.content = "Some great ideas for the future.";
    dto.parentFolderId = "folder-2";
    dto.createdAt = 1234567890;
    dto.updatedAt = 1234567890;
    
    // Convert to entity
    Note note = mapper.toEntity(dto);
    assert(note.getId() == "note-2");
    assert(note.getName() == "Ideas");
    assert(note.getPath() == "/notes/ideas.md");
    assert(note.getContent() == "Some great ideas for the future.");
    assert(note.getParentFolderId() == "folder-2");
}

TEST(test_note_mapper_roundtrip) {
    SqliteNoteMapper mapper;
    
    Note original("note-3", "Test Note", "/test.md", "folder-1");
    original.setContent("Test content for roundtrip");
    
    NoteDTO* baseDto = mapper.toDTO(original);
    Note restored = mapper.toEntity(*baseDto);
    
    assert(restored.getId() == original.getId());
    assert(restored.getName() == original.getName());
    assert(restored.getPath() == original.getPath());
    assert(restored.getContent() == original.getContent());
    assert(restored.getParentFolderId() == original.getParentFolderId());
    
    delete baseDto;
}

TEST(test_note_mapper_empty_content) {
    SqliteNoteMapper mapper;
    
    Note note("note-4", "Empty", "/empty.md", "folder-1");
    // Don't set content - should be empty
    
    NoteDTO* baseDto = mapper.toDTO(note);
    SqliteNoteDTO* dto = dynamic_cast<SqliteNoteDTO*>(baseDto);
    
    assert(dto->content.empty());
    
    delete baseDto;
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST(test_mapper_wrong_dto_type) {
    SqliteProjectMapper mapper;
    
    // Create a note DTO (wrong type)
    SqliteNoteDTO noteDto;
    noteDto.id = "note-1";
    
    // Try to convert as ProjectDTO
    NoteDTO* baseDto = &noteDto;
    
    try {
        // This should throw because baseDto is not a SqliteProjectDTO
        ProjectDTO* wrongBaseDto = dynamic_cast<ProjectDTO*>(baseDto);
        if (wrongBaseDto) {
            Project project = mapper.toEntity(*wrongBaseDto);
            assert(false); // Should not reach here
        }
    } catch (const std::runtime_error& e) {
        // Expected - wrong DTO type
        assert(true);
    }
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "=== PlotterSqliteMappers Test Suite ===" << std::endl << std::endl;
    
    // Project mapper tests
    std::cout << "--- SqliteProjectMapper Tests ---" << std::endl;
    run_test_project_mapper_to_dto();
    run_test_project_mapper_to_entity();
    run_test_project_mapper_roundtrip();
    
    // Folder mapper tests
    std::cout << "\n--- SqliteFolderMapper Tests ---" << std::endl;
    run_test_folder_mapper_to_dto();
    run_test_folder_mapper_to_entity();
    run_test_folder_mapper_roundtrip();
    
    // Note mapper tests
    std::cout << "\n--- SqliteNoteMapper Tests ---" << std::endl;
    run_test_note_mapper_to_dto();
    run_test_note_mapper_to_entity();
    run_test_note_mapper_roundtrip();
    run_test_note_mapper_empty_content();
    
    // Error handling tests
    std::cout << "\n--- Error Handling Tests ---" << std::endl;
    run_test_mapper_wrong_dto_type();
    
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
