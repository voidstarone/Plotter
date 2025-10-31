#include "plotter_filesystem/FilesystemDataSource.h"
#include "plotter_filesystem_dtos/FilesystemDTOs.h"
#include <iostream>
#include <cassert>
#include <filesystem>

using namespace plotter::filesystem;
using namespace plotter::filesystem_dtos;
namespace fs = std::filesystem;

const std::string TEST_ROOT = "/tmp/plotter_test_datasource";

void cleanup() {
    if (fs::exists(TEST_ROOT)) {
        fs::remove_all(TEST_ROOT);
    }
}

void setup() {
    cleanup();
    fs::create_directories(TEST_ROOT);
}

void testProjectDataSourceConnection() {
    FilesystemProjectDataSource ds("test-project-ds", TEST_ROOT);

    assert(!ds.isConnected());
    ds.connect();
    assert(ds.isConnected());
    assert(ds.getName() == "test-project-ds");
    ds.disconnect();
    assert(!ds.isConnected());

    std::cout << "✓ Project DataSource connection test passed\n";
}

void testProjectCreate() {
    FilesystemProjectDataSource ds("test-project-ds", TEST_ROOT);
    ds.connect();

    FilesystemProjectDTO dto;
    dto.name = "TestProject";
    dto.description = "A test project";
    dto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    dto.updatedAt = dto.createdAt;

    std::string projectId = ds.create(&dto);
    assert(!projectId.empty());
    assert(!dto.id.empty());

    // Verify the directory was created
    std::string projectPath = TEST_ROOT + "/TestProject";
    assert(fs::exists(projectPath));
    assert(fs::exists(projectPath + "/.plotter_project"));

    ds.disconnect();
    std::cout << "✓ Project create test passed (ID: " << projectId << ")\n";
}

void testProjectReadAndList() {
    FilesystemProjectDataSource ds("test-project-ds", TEST_ROOT);
    ds.connect();

    // Create a project
    FilesystemProjectDTO createDto;
    createDto.name = "ReadTestProject";
    createDto.description = "Test reading";
    createDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    createDto.updatedAt = createDto.createdAt;
    std::string projectId = ds.create(&createDto);

    // Read it back
    auto* readDto = dynamic_cast<FilesystemProjectDTO*>(ds.read(projectId));
    assert(readDto != nullptr);
    assert(readDto->id == projectId);
    assert(readDto->name == "ReadTestProject");
    assert(readDto->description == "Test reading");
    delete readDto;

    // List all projects
    auto projects = ds.list();
    assert(!projects.empty());
    bool found = false;
    for (auto* proj : projects) {
        auto* fsProj = dynamic_cast<FilesystemProjectDTO*>(proj);
        if (fsProj && fsProj->id == projectId) {
            found = true;
        }
        delete proj;
    }
    assert(found);

    ds.disconnect();
    std::cout << "✓ Project read and list test passed\n";
}

void testFolderDataSource() {
    // First create a project
    FilesystemProjectDataSource projectDs("test-project-ds", TEST_ROOT);
    projectDs.connect();

    FilesystemProjectDTO projectDto;
    projectDto.name = "FolderTestProject";
    projectDto.description = "Project for folder testing";
    projectDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    projectDto.updatedAt = projectDto.createdAt;
    std::string projectId = projectDs.create(&projectDto);
    projectDs.disconnect();

    // Now create a folder in the project
    FilesystemFolderDataSource folderDs("test-folder-ds", TEST_ROOT);
    folderDs.connect();

    FilesystemFolderDTO folderDto;
    folderDto.name = "TestFolder";
    folderDto.description = "A test folder";
    folderDto.parentProjectId = projectId;
    folderDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    folderDto.updatedAt = folderDto.createdAt;

    std::string folderId = folderDs.create(&folderDto);
    assert(!folderId.empty());

    // Read it back
    auto* readFolder = dynamic_cast<FilesystemFolderDTO*>(folderDs.read(folderId));
    assert(readFolder != nullptr);
    assert(readFolder->id == folderId);
    assert(readFolder->name == "TestFolder");
    delete readFolder;

    // List folders in project
    auto folders = folderDs.listByProject(projectId);
    assert(!folders.empty());
    for (auto* folder : folders) {
        delete folder;
    }

    folderDs.disconnect();
    std::cout << "✓ Folder DataSource test passed\n";
}

void testNoteDataSource() {
    // First create a project and folder
    FilesystemProjectDataSource projectDs("test-project-ds", TEST_ROOT);
    projectDs.connect();

    FilesystemProjectDTO projectDto;
    projectDto.name = "NoteTestProject";
    projectDto.description = "Project for note testing";
    projectDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    projectDto.updatedAt = projectDto.createdAt;
    std::string projectId = projectDs.create(&projectDto);
    projectDs.disconnect();

    FilesystemFolderDataSource folderDs("test-folder-ds", TEST_ROOT);
    folderDs.connect();

    FilesystemFolderDTO folderDto;
    folderDto.name = "NotesFolder";
    folderDto.description = "Folder for notes";
    folderDto.parentProjectId = projectId;
    folderDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    folderDto.updatedAt = folderDto.createdAt;
    std::string folderId = folderDs.create(&folderDto);
    folderDs.disconnect();

    // Now create a note
    FilesystemNoteDataSource noteDs("test-note-ds", TEST_ROOT);
    noteDs.connect();

    FilesystemNoteDTO noteDto;
    noteDto.name = "TestNote";
    noteDto.content = "# Test Note\n\nThis is a test note.";
    noteDto.parentFolderId = folderId;
    noteDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    noteDto.updatedAt = noteDto.createdAt;

    std::string noteId = noteDs.create(&noteDto);
    assert(!noteId.empty());

    // Read it back
    auto* readNote = dynamic_cast<FilesystemNoteDTO*>(noteDs.read(noteId));
    assert(readNote != nullptr);
    assert(readNote->id == noteId);
    assert(readNote->name == "TestNote");
    assert(readNote->content == "# Test Note\n\nThis is a test note.");
    delete readNote;

    // Test content operations
    std::string content = noteDs.getContent(noteId);
    assert(content == "# Test Note\n\nThis is a test note.");

    bool updated = noteDs.updateContent(noteId, "# Updated Note\n\nNew content!");
    assert(updated);

    std::string newContent = noteDs.getContent(noteId);
    assert(newContent == "# Updated Note\n\nNew content!");

    // List notes in folder
    auto notes = noteDs.listByFolder(folderId);
    assert(!notes.empty());
    for (auto* note : notes) {
        delete note;
    }

    noteDs.disconnect();
    std::cout << "✓ Note DataSource test passed\n";
}

void testDataSourceIntegration() {
    // Test full workflow: create project -> folder -> note
    FilesystemProjectDataSource projectDs("fs-project", TEST_ROOT);
    FilesystemFolderDataSource folderDs("fs-folder", TEST_ROOT);
    FilesystemNoteDataSource noteDs("fs-note", TEST_ROOT);

    projectDs.connect();
    folderDs.connect();
    noteDs.connect();

    // Create project
    FilesystemProjectDTO projectDto;
    projectDto.name = "IntegrationTestProject";
    projectDto.description = "Full integration test";
    projectDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    projectDto.updatedAt = projectDto.createdAt;
    std::string projectId = projectDs.create(&projectDto);

    // Create folder
    FilesystemFolderDTO folderDto;
    folderDto.name = "MainFolder";
    folderDto.description = "Main folder";
    folderDto.parentProjectId = projectId;
    folderDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    folderDto.updatedAt = folderDto.createdAt;
    std::string folderId = folderDs.create(&folderDto);

    // Create note
    FilesystemNoteDTO noteDto;
    noteDto.name = "FirstNote";
    noteDto.content = "Hello, Plotter!";
    noteDto.parentFolderId = folderId;
    noteDto.createdAt = FilesystemDTOUtils::getCurrentTimestamp();
    noteDto.updatedAt = noteDto.createdAt;
    std::string noteId = noteDs.create(&noteDto);

    // Verify structure
    std::string expectedPath = TEST_ROOT + "/IntegrationTestProject/MainFolder/FirstNote.md";
    assert(fs::exists(expectedPath));
    assert(fs::exists(expectedPath + ".plotter_meta"));

    projectDs.disconnect();
    folderDs.disconnect();
    noteDs.disconnect();

    std::cout << "✓ DataSource integration test passed\n";
}

int main() {
    std::cout << "Running PlotterFilesystemDataSource tests...\n\n";

    try {
        setup();

        testProjectDataSourceConnection();
        cleanup(); setup();

        testProjectCreate();
        cleanup(); setup();

        testProjectReadAndList();
        cleanup(); setup();

        testFolderDataSource();
        cleanup(); setup();

        testNoteDataSource();
        cleanup(); setup();

        testDataSourceIntegration();
        cleanup();

        std::cout << "\n✅ All tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << "\n";
        cleanup();
        return 1;
    }
}
