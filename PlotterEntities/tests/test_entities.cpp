#include <iostream>
#include <cassert>
#include <stdexcept>
#include "Project.h"
#include "Folder.h"
#include "Note.h"

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
// Project Tests
// ============================================================================

TEST(test_project_constructor) {
    Project project("proj-1", "My Project", "A test project");
    assert(project.getId() == "proj-1");
    assert(project.getName() == "My Project");
    assert(project.getDescription() == "A test project");
    assert(project.getType() == "project");
    assert(project.getFolderIds().empty());
}

TEST(test_project_add_folder) {
    Project project("proj-1", "My Project", "Test");
    
    project.addFolderId("folder-1");
    assert(project.getFolderIds().size() == 1);
    assert(project.getFolderIds()[0] == "folder-1");
    
    project.addFolderId("folder-2");
    assert(project.getFolderIds().size() == 2);
    assert(project.getFolderIds()[1] == "folder-2");
}

TEST(test_project_remove_folder) {
    Project project("proj-1", "My Project", "Test");
    project.addFolderId("folder-1");
    project.addFolderId("folder-2");
    project.addFolderId("folder-3");
    
    bool removed = project.removeFolderId("folder-2");
    assert(removed == true);
    assert(project.getFolderIds().size() == 2);
    
    // Verify folder-2 is gone and others remain
    bool found = false;
    for (const auto& id : project.getFolderIds()) {
        if (id == "folder-2") found = true;
    }
    assert(found == false);
    
    // Try removing non-existent folder
    bool removed2 = project.removeFolderId("folder-999");
    assert(removed2 == false);
    assert(project.getFolderIds().size() == 2);
}

TEST(test_project_set_description) {
    Project project("proj-1", "My Project", "Original");
    project.setDescription("Updated description");
    assert(project.getDescription() == "Updated description");
}

// ============================================================================
// Folder Tests
// ============================================================================

TEST(test_folder_constructor) {
    Folder folder("folder-1", "Documents", "My documents", "proj-1", "");
    assert(folder.getId() == "folder-1");
    assert(folder.getName() == "Documents");
    assert(folder.getDescription() == "My documents");
    assert(folder.getParentProjectId() == "proj-1");
    assert(folder.getParentFolderId() == "");
    assert(folder.getType() == "folder");
    assert(folder.getNoteIds().empty());
    assert(folder.getSubfolderIds().empty());
}

TEST(test_folder_add_note) {
    Folder folder("folder-1", "Documents", "Test", "proj-1", "");
    
    folder.addNoteId("note-1");
    assert(folder.getNoteIds().size() == 1);
    assert(folder.getNoteIds()[0] == "note-1");
    
    folder.addNoteId("note-2");
    assert(folder.getNoteIds().size() == 2);
}

TEST(test_folder_remove_note) {
    Folder folder("folder-1", "Documents", "Test", "proj-1", "");
    folder.addNoteId("note-1");
    folder.addNoteId("note-2");
    folder.addNoteId("note-3");
    
    bool removed = folder.removeNoteId("note-2");
    assert(removed == true);
    assert(folder.getNoteIds().size() == 2);
    
    bool removed2 = folder.removeNoteId("note-999");
    assert(removed2 == false);
}

TEST(test_folder_add_subfolder) {
    Folder folder("folder-1", "Documents", "Test", "proj-1", "");
    
    folder.addSubfolderId("subfolder-1");
    assert(folder.getSubfolderIds().size() == 1);
    
    folder.addSubfolderId("subfolder-2");
    assert(folder.getSubfolderIds().size() == 2);
}

TEST(test_folder_remove_subfolder) {
    Folder folder("folder-1", "Documents", "Test", "proj-1", "");
    folder.addSubfolderId("sub-1");
    folder.addSubfolderId("sub-2");
    
    bool removed = folder.removeSubfolderId("sub-1");
    assert(removed == true);
    assert(folder.getSubfolderIds().size() == 1);
    assert(folder.getSubfolderIds()[0] == "sub-2");
}

TEST(test_folder_set_parent) {
    Folder folder("folder-1", "Documents", "Test", "proj-1", "");
    
    folder.setParentFolderId("parent-folder");
    assert(folder.getParentFolderId() == "parent-folder");
    
    folder.setParentProjectId("proj-2");
    assert(folder.getParentProjectId() == "proj-2");
}

// ============================================================================
// Note Tests
// ============================================================================

TEST(test_note_constructor) {
    Note note("note-1", "Meeting Notes", "/notes/meeting.md", "folder-1");
    assert(note.getId() == "note-1");
    assert(note.getName() == "Meeting Notes");
    assert(note.getPath() == "/notes/meeting.md");
    assert(note.getParentFolderId() == "folder-1");
    assert(note.getType() == "note");
    assert(note.getContent().empty());
}

TEST(test_note_set_content) {
    Note note("note-1", "My Note", "/notes/test.md", "folder-1");
    
    note.setContent("This is the note content");
    assert(note.getContent() == "This is the note content");
    
    note.setContent("Updated content");
    assert(note.getContent() == "Updated content");
}

TEST(test_note_set_path) {
    Note note("note-1", "My Note", "/notes/test.md", "folder-1");
    
    note.setPath("/new/path/note.md");
    assert(note.getPath() == "/new/path/note.md");
}

TEST(test_note_set_parent_folder) {
    Note note("note-1", "My Note", "/notes/test.md", "folder-1");
    
    note.setParentFolderId("folder-2");
    assert(note.getParentFolderId() == "folder-2");
}

TEST(test_note_attributes) {
    Note note("note-1", "My Note", "/notes/test.md", "folder-1");
    
    // Set string attribute
    note.setAttribute("author", std::string("John Doe"));
    assert(note.hasAttribute("author"));
    assert(note.getAttribute<std::string>("author") == "John Doe");
    
    // Set int attribute
    note.setAttribute("priority", 5);
    assert(note.hasAttribute("priority"));
    assert(note.getAttribute<int>("priority") == 5);
    
    // Set bool attribute
    note.setAttribute("archived", true);
    assert(note.hasAttribute("archived"));
    assert(note.getAttribute<bool>("archived") == true);
    
    // Get with default
    int defaultValue = note.getAttribute<int>("non-existent", 99);
    assert(defaultValue == 99);
    
    // Remove attribute
    bool removed = note.removeAttribute("priority");
    assert(removed == true);
    assert(!note.hasAttribute("priority"));
    
    // Try removing non-existent
    bool removed2 = note.removeAttribute("non-existent");
    assert(removed2 == false);
}

TEST(test_note_attribute_type_mismatch) {
    Note note("note-1", "My Note", "/notes/test.md", "folder-1");
    note.setAttribute("count", 42);
    
    try {
        // Try to get as wrong type
        std::string value = note.getAttribute<std::string>("count");
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        // Expected exception
        assert(true);
    }
}

TEST(test_note_missing_attribute) {
    Note note("note-1", "My Note", "/notes/test.md", "folder-1");
    
    try {
        int value = note.getAttribute<int>("missing");
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        // Expected exception
        assert(true);
    }
}

TEST(test_note_timestamps) {
    Note note("note-1", "My Note", "/notes/test.md", "folder-1");
    
    auto created = note.getCreatedAt();
    auto updated = note.getUpdatedAt();
    
    // Initially, created and updated should be close
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(updated - created).count();
    assert(diff >= 0 && diff < 2);
    
    // Modifying content should update timestamp
    note.setContent("New content");
    auto newUpdated = note.getUpdatedAt();
    assert(newUpdated >= updated);
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "=== PlotterEntities Test Suite ===" << std::endl << std::endl;
    
    // Project tests
    std::cout << "--- Project Tests ---" << std::endl;
    run_test_project_constructor();
    run_test_project_add_folder();
    run_test_project_remove_folder();
    run_test_project_set_description();
    
    // Folder tests
    std::cout << "\n--- Folder Tests ---" << std::endl;
    run_test_folder_constructor();
    run_test_folder_add_note();
    run_test_folder_remove_note();
    run_test_folder_add_subfolder();
    run_test_folder_remove_subfolder();
    run_test_folder_set_parent();
    
    // Note tests
    std::cout << "\n--- Note Tests ---" << std::endl;
    run_test_note_constructor();
    run_test_note_set_content();
    run_test_note_set_path();
    run_test_note_set_parent_folder();
    run_test_note_attributes();
    run_test_note_attribute_type_mismatch();
    run_test_note_missing_attribute();
    run_test_note_timestamps();
    
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
