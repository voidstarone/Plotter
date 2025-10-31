#include <iostream>
#include <memory>
#include <unordered_map>
#include "Project.h"
#include "Folder.h"
#include "Note.h"
#include "FileItem.h"
#include "NoteStorage.h"
#include "FileSystemNoteStorage.h"

int main() {
    std::cout << "=== NoteTaker Simple Entity Demo ===\n\n";

    // Create storage backend
    auto storage = std::make_shared<FileSystemNoteStorage>("./notes_data");
    std::cout << "Created filesystem storage at ./notes_data\n\n";

    // Create entities with explicit IDs
    // In a real system, these would be managed by a repository or database
    
    // Create a project
    auto project = std::make_shared<Project>("proj_001", "My Research", "Research notes collection");
    std::cout << "Created Project: " << project->getName() << " (ID: " << project->getId() << ")\n";
    std::cout << "  Type: " << project->getType() << "\n";
    std::cout << "  Description: " << project->getDescription() << "\n\n";

    // Create folders
    auto folder1 = std::make_shared<Folder>("folder_001", "Literature Review", 
                                             "Papers and articles", "proj_001", "");
    auto folder2 = std::make_shared<Folder>("folder_002", "Experiments", 
                                             "Experimental data", "proj_001", "");
    
    std::cout << "Created Folder: " << folder1->getName() << " (ID: " << folder1->getId() << ")\n";
    std::cout << "  Type: " << folder1->getType() << "\n";
    std::cout << "  Parent Project ID: " << folder1->getParentProjectId() << "\n\n";
    
    std::cout << "Created Folder: " << folder2->getName() << " (ID: " << folder2->getId() << ")\n";
    std::cout << "  Type: " << folder2->getType() << "\n";
    std::cout << "  Parent Project ID: " << folder2->getParentProjectId() << "\n\n";

    // Link folders to project (ID-based relationship)
    project->addFolderId(folder1->getId());
    project->addFolderId(folder2->getId());

    // Create a subfolder
    auto subfolder = std::make_shared<Folder>("folder_003", "Methods",
                                               "Experimental methods", "proj_001", "folder_002");
    
    std::cout << "Created Subfolder: " << subfolder->getName() << " (ID: " << subfolder->getId() << ")\n";
    std::cout << "  Parent Folder ID: " << subfolder->getParentFolderId() << "\n\n";
    
    // Link subfolder to parent folder
    folder2->addSubfolderId(subfolder->getId());

    // Create notes
    auto note1 = std::make_shared<Note>("note_001", "Paper 1", "lit/paper1.txt",
                                         "folder_001", storage);
    auto note2 = std::make_shared<Note>("note_002", "Paper 2", "lit/paper2.txt",
                                         "folder_001", storage);
    auto note3 = std::make_shared<Note>("note_003", "Setup", "exp/setup.txt",
                                         "folder_003", storage);
    
    std::cout << "Created Note: " << note1->getName() << " (ID: " << note1->getId() << ")\n";
    std::cout << "  Type: " << note1->getType() << "\n";
    std::cout << "  Parent Folder ID: " << note1->getParentFolderId() << "\n";
    std::cout << "  Storage Path: " << note1->getPath() << "\n\n";

    // Save some content to notes
    note1->setContent("This paper discusses machine learning techniques...");
    note2->setContent("An analysis of neural network architectures...");
    note3->setContent("Materials: 10kg copper wire, 5L distilled water...");

    // Link notes to folders (ID-based relationship)
    folder1->addNoteId(note1->getId());
    folder1->addNoteId(note2->getId());
    subfolder->addNoteId(note3->getId());

    // Add attributes to a note
    note1->setAttribute<std::string>("author", "Smith et al.");
    note1->setAttribute<int>("year", 2023);
    note1->setAttribute<double>("rating", 4.5);

    // Display the ID-based structure
    std::cout << "=== ID-Based Relationship Structure ===\n\n";
    
    std::cout << "Project '" << project->getName() << "' contains folder IDs:\n";
    for (const auto& folderId : project->getFolderIds()) {
        std::cout << "  - " << folderId << "\n";
    }
    std::cout << "\n";

    std::cout << "Folder '" << folder1->getName() << "' contains note IDs:\n";
    for (const auto& noteId : folder1->getNoteIds()) {
        std::cout << "  - " << noteId << "\n";
    }
    std::cout << "\n";

    std::cout << "Folder '" << folder2->getName() << "' contains subfolder IDs:\n";
    for (const auto& subfolderId : folder2->getSubfolderIds()) {
        std::cout << "  - " << subfolderId << "\n";
    }
    std::cout << "\n";

    std::cout << "Subfolder '" << subfolder->getName() << "' contains note IDs:\n";
    for (const auto& noteId : subfolder->getNoteIds()) {
        std::cout << "  - " << noteId << "\n";
    }
    std::cout << "\n";

    // Demonstrate entity is just getters/setters
    std::cout << "=== Demonstrating Entity Mutations ===\n\n";
    
    std::cout << "Renaming folder '" << folder1->getName() << "' to 'Research Papers'\n";
    folder1->setName("Research Papers");
    std::cout << "New name: " << folder1->getName() << "\n\n";

    std::cout << "Moving note '" << note2->getName() << "' from folder " 
              << note2->getParentFolderId() << " to " << subfolder->getId() << "\n";
    folder1->removeNoteId(note2->getId());
    note2->setParentFolderId(subfolder->getId());
    subfolder->addNoteId(note2->getId());
    std::cout << "Note's new parent: " << note2->getParentFolderId() << "\n\n";

    // Demonstrate lazy loading
    std::cout << "=== Demonstrating Lazy Loading ===\n";
    std::cout << "Accessing note1 content: " << note1->getContent() << "\n";
    std::cout << "Note1 author attribute: " << note1->getAttribute<std::string>("author") << "\n";
    std::cout << "Note1 year attribute: " << note1->getAttribute<int>("year") << "\n\n";

    std::cout << "=== Summary ===\n";
    std::cout << "All entities are simple objects with IDs.\n";
    std::cout << "Relationships are maintained through ID references.\n";
    std::cout << "No embedded objects - just getters and setters.\n";
    std::cout << "Content is still loaded lazily from storage.\n";

    return 0;
}
