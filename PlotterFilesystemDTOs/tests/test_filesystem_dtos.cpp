#include "plotter_filesystem_dtos/FilesystemDTOs.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>

using namespace plotter::filesystem_dtos;
namespace fs = std::filesystem;

void testTimestamp() {
    long long ts1 = FilesystemDTOUtils::getCurrentTimestamp();
    long long ts2 = FilesystemDTOUtils::getCurrentTimestamp();

    assert(ts1 > 0);
    assert(ts2 >= ts1);
    std::cout << "✓ Timestamp test passed\n";
}

void testGenerateId() {
    std::string id1 = FilesystemDTOUtils::generateId();
    std::string id2 = FilesystemDTOUtils::generateId();

    assert(!id1.empty());
    assert(!id2.empty());
    assert(id1 != id2);
    assert(id1.length() >= 32); // UUID-like format
    std::cout << "✓ Generate ID test passed (sample: " << id1 << ")\n";
}

void testDotfileReadWrite() {
    std::string testPath = "/tmp/test_plotter_dotfile";
    std::string testContent = "test content 123";

    FilesystemDTOUtils::writeDotfile(testPath, testContent);
    std::string readContent = FilesystemDTOUtils::readDotfile(testPath);

    assert(readContent == testContent);
    fs::remove(testPath);
    std::cout << "✓ Dotfile read/write test passed\n";
}

void testIsProjectDirectory() {
    fs::path tempDir = fs::temp_directory_path() / "test_plotter_project";
    fs::create_directory(tempDir);

    // Should be false before creating dotfile
    assert(!FilesystemDTOUtils::isProjectDirectory(tempDir.string()));

    // Create dotfile
    std::ofstream dotfile(tempDir / ".plotter_project");
    dotfile << "test";
    dotfile.close();

    // Should be true now
    assert(FilesystemDTOUtils::isProjectDirectory(tempDir.string()));

    // Cleanup
    fs::remove_all(tempDir);
    std::cout << "✓ Is project directory test passed\n";
}

void testFilesystemProjectDTO() {
    FilesystemProjectDTO dto;
    dto.id = "test-id";
    dto.name = "Test Project";
    dto.description = "Test description";
    dto.rootPath = "/tmp/test";
    dto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    dto.updatedAt = dto.createdAt;
    dto.folderIds.push_back("folder1");
    dto.folderIds.push_back("folder2");

    assert(dto.id == "test-id");
    assert(dto.folderIds.size() == 2);
    std::cout << "✓ FilesystemProjectDTO test passed\n";
}

void testFilesystemFolderDTO() {
    FilesystemFolderDTO dto;
    dto.id = "folder-id";
    dto.name = "Test Folder";
    dto.description = "Test folder description";
    dto.path = "/tmp/test/folder";
    dto.parentProjectId = "project-id";
    dto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    dto.updatedAt = dto.createdAt;

    assert(dto.id == "folder-id");
    assert(dto.parentProjectId == "project-id");
    std::cout << "✓ FilesystemFolderDTO test passed\n";
}

void testFilesystemNoteDTO() {
    FilesystemNoteDTO dto;
    dto.id = "note-id";
    dto.name = "Test Note";
    dto.path = "/tmp/test/note.md";
    dto.content = "# Test Note\n\nContent here";
    dto.parentFolderId = "folder-id";
    dto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    dto.updatedAt = dto.createdAt;

    assert(dto.id == "note-id");
    assert(dto.content.find("Test Note") != std::string::npos);
    std::cout << "✓ FilesystemNoteDTO test passed\n";
}

int main() {
    std::cout << "Running PlotterFilesystemDTOs tests...\n\n";

    try {
        testTimestamp();
        testGenerateId();
        testDotfileReadWrite();
        testIsProjectDirectory();
        testFilesystemProjectDTO();
        testFilesystemFolderDTO();
        testFilesystemNoteDTO();

        std::cout << "\n✅ All tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << "\n";
        return 1;
    }
}
cmake_minimum_required(VERSION 3.20)

# Add test executable
add_executable(test_filesystem_dtos
    test_filesystem_dtos.cpp
)

# Link against the library
target_link_libraries(test_filesystem_dtos
    PRIVATE
        PlotterFilesystemDTOs
)

# Add test
add_test(NAME FilesystemDTOsTest COMMAND test_filesystem_dtos)

