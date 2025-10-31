#include "plotter_sqlite_mappers/SqliteMappers.h"
#include "plotter_sqlite_dtos/SqliteDTOs.h"
#include <iostream>
#include <cassert>

using namespace plotter::sqlite_mappers;
using namespace plotter::sqlite_dtos;

// Note: This is a basic structural test without full entity library integration

void testMapperInstantiation() {
    SqliteProjectMapper projectMapper;
    SqliteFolderMapper folderMapper;
    SqliteNoteMapper noteMapper;

    std::cout << "✓ Mapper instantiation test passed\n";
}

void testDTOCreation() {
    SqliteProjectDTO projectDto;
    projectDto.id = "test-project-123";
    projectDto.name = "Test Project";
    projectDto.description = "A test project";
    projectDto.createdAt = 1234567890000LL;
    projectDto.updatedAt = 1234567890000LL;

    assert(projectDto.id == "test-project-123");
    assert(projectDto.name == "Test Project");

    SqliteFolderDTO folderDto;
    folderDto.id = "test-folder-456";
    folderDto.name = "Test Folder";
    folderDto.parentProjectId = "test-project-123";

    assert(folderDto.id == "test-folder-456");
    assert(folderDto.parentProjectId == "test-project-123");

    SqliteNoteDTO noteDto;
    noteDto.id = "test-note-789";
    noteDto.name = "Test Note";
    noteDto.path = "/tmp/test-project/folder/note.md";
    noteDto.parentFolderId = "test-folder-456";

    assert(noteDto.id == "test-note-789");
    assert(noteDto.parentFolderId == "test-folder-456");

    std::cout << "✓ DTO creation and field assignment test passed\n";
}

int main() {
    std::cout << "Running PlotterSqliteMappers tests...\n\n";

    try {
        testMapperInstantiation();
        testDTOCreation();

        std::cout << "\n✅ All structural tests passed!\n";
        std::cout << "Note: Full integration tests require entity library linkage.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << "\n";
        return 1;
    }
}
