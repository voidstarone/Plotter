#include "plotter_filesystem_mappers/FilesystemMappers.h"
#include "plotter_filesystem_dtos/FilesystemDTOs.h"
#include <iostream>
#include <cassert>

using namespace plotter::filesystem_mappers;
using namespace plotter::filesystem_dtos;

// Note: This is a basic structural test without full entity library integration

void testMapperInstantiation() {
    FilesystemProjectMapper projectMapper;
    FilesystemFolderMapper folderMapper;
    FilesystemNoteMapper noteMapper;

    std::cout << "✓ Mapper instantiation test passed\n";
}

void testDTOCreation() {
    FilesystemProjectDTO projectDto;
    projectDto.id = "test-project-123";
    projectDto.name = "Test Project";
    projectDto.description = "A test project";
    projectDto.rootPath = "/tmp/test-project";
    projectDto.createdAt = 1234567890000LL;
    projectDto.updatedAt = 1234567890000LL;

    assert(projectDto.id == "test-project-123");
    assert(projectDto.name == "Test Project");

    FilesystemFolderDTO folderDto;
    folderDto.id = "test-folder-456";
    folderDto.name = "Test Folder";
    folderDto.parentProjectId = "test-project-123";

    assert(folderDto.id == "test-folder-456");
    assert(folderDto.parentProjectId == "test-project-123");

    FilesystemNoteDTO noteDto;
    noteDto.id = "test-note-789";
    noteDto.name = "Test Note";
    noteDto.path = "/tmp/test-project/folder/note.md";
    noteDto.parentFolderId = "test-folder-456";

    assert(noteDto.id == "test-note-789");
    assert(noteDto.parentFolderId == "test-folder-456");

    std::cout << "✓ DTO creation and field assignment test passed\n";
}

int main() {
    std::cout << "Running PlotterFilesystemMappers tests...\n\n";

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

