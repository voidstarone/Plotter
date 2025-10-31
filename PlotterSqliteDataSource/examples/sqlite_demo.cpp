#include <iostream>
#include <memory>
#include "plotter_sqlite/SqliteProjectDataSource.h"
#include "plotter_sqlite/SqliteFolderDataSource.h"
#include "plotter_sqlite/SqliteNoteDataSource.h"
#include "Project.h"
#include "Folder.h"
#include "Note.h"
#include "FileSystemNoteStorage.h"

using namespace plotter::sqlite;

int main() {
    std::cout << "=== SQLite DataSource Demo ===" << std::endl << std::endl;

    const std::string dbPath = "./demo_plotter.db";
    const std::string notesDir = "./demo_notes";

    try {
        // ================================================================
        // 1. Create and connect the SQLite datasources
        // ================================================================
        std::cout << "1. Creating SQLite datasources..." << std::endl;
        
        SqliteProjectDataSource projectDS("sqlite-projects", dbPath, 100);
        SqliteFolderDataSource folderDS("sqlite-folders", dbPath, 100);
        SqliteNoteDataSource noteDS("sqlite-notes", dbPath, 100);
        
        projectDS.connect();
        folderDS.connect();
        noteDS.connect();
        
        std::cout << "   ✓ All datasources connected" << std::endl << std::endl;

        // ================================================================
        // 2. Check datasource health
        // ================================================================
        std::cout << "2. Checking datasource health..." << std::endl;
        
        auto projectHealth = projectDS.checkHealth();
        auto folderHealth = folderDS.checkHealth();
        auto noteHealth = noteDS.checkHealth();
        
        std::cout << "   Project DS: " << (projectHealth.isHealthy() ? "HEALTHY" : "UNHEALTHY") 
                  << " - " << projectHealth.message << std::endl;
        std::cout << "   Folder DS:  " << (folderHealth.isHealthy() ? "HEALTHY" : "UNHEALTHY") 
                  << " - " << folderHealth.message << std::endl;
        std::cout << "   Note DS:    " << (noteHealth.isHealthy() ? "HEALTHY" : "UNHEALTHY") 
                  << " - " << noteHealth.message << std::endl << std::endl;

        // ================================================================
        // 3. Create and save a Project
        // ================================================================
        std::cout << "3. Creating a project..." << std::endl;
        
        Project research("proj-001", "AI Research", "Research on artificial intelligence");
        std::string savedProjectId = projectDS.save(research);
        
        std::cout << "   ✓ Project saved with ID: " << savedProjectId << std::endl << std::endl;

        // ================================================================
        // 4. Create and save Folders
        // ================================================================
        std::cout << "4. Creating folders..." << std::endl;
        
        // Top-level folder under project
        Folder papers("folder-001", "Papers", "Research papers", "proj-001", "");
        folderDS.save(papers);
        std::cout << "   ✓ Created folder: " << papers.getName() << std::endl;
        
        // Subfolder under "Papers"
        Folder machineLearn("folder-002", "Machine Learning", "ML papers", "", "folder-001");
        folderDS.save(machineLearn);
        std::cout << "   ✓ Created subfolder: " << machineLearn.getName() << std::endl;
        
        // Another top-level folder
        Folder code("folder-003", "Code", "Source code", "proj-001", "");
        folderDS.save(code);
        std::cout << "   ✓ Created folder: " << code.getName() << std::endl << std::endl;

        // ================================================================
        // 5. Create and save Notes with FileSystemNoteStorage
        // ================================================================
        std::cout << "5. Creating notes..." << std::endl;
        
        auto noteStorage = std::make_shared<FileSystemNoteStorage>(notesDir);
        
        Note paper1("note-001", "Attention is All You Need", "ml/attention.md", "folder-002");
        paper1.setContent("# Attention is All You Need\n\nTransformers architecture paper...");
        noteDS.save(paper1);
        std::cout << "   ✓ Created note: " << paper1.getName() << std::endl;
        
        Note paper2("note-002", "BERT Paper", "ml/bert.md", "folder-002");
        paper2.setContent("# BERT\n\nBidirectional Encoder Representations from Transformers...");
        noteDS.save(paper2);
        std::cout << "   ✓ Created note: " << paper2.getName() << std::endl;
        
        Note codeNote("note-003", "Training Script", "training.py", "folder-003");
        codeNote.setContent("# Training script\nimport torch\n\n...");
        noteDS.save(codeNote);
        std::cout << "   ✓ Created note: " << codeNote.getName() << std::endl << std::endl;

        // ================================================================
        // 6. Query and demonstrate relational integrity
        // ================================================================
        std::cout << "6. Querying data (demonstrating relational queries)..." << std::endl;
        
        // Find project by ID
        auto retrievedProject = projectDS.findById("proj-001");
        if (retrievedProject) {
            std::cout << "   Project: " << retrievedProject->getName() << std::endl;
            std::cout << "   Description: " << retrievedProject->getDescription() << std::endl;
            
            // Note: Folder IDs are populated via JOIN query from folders table
            auto folderIds = retrievedProject->getFolderIds();
            std::cout << "   Top-level folders: " << folderIds.size() << std::endl;
        }
        std::cout << std::endl;

        // Find folders by project
        std::cout << "   Folders in project 'proj-001':" << std::endl;
        auto projectFolders = folderDS.findByProjectId("proj-001");
        for (const auto& folder : projectFolders) {
            std::cout << "   - " << folder.getName() << " (" << folder.getId() << ")" << std::endl;
        }
        std::cout << std::endl;

        // Find subfolders
        std::cout << "   Subfolders of 'Papers' folder:" << std::endl;
        auto subfolders = folderDS.findByParentFolderId("folder-001");
        for (const auto& subfolder : subfolders) {
            std::cout << "   - " << subfolder.getName() << std::endl;
        }
        std::cout << std::endl;

        // Find notes in a folder
        std::cout << "   Notes in 'Machine Learning' folder:" << std::endl;
        auto notes = noteDS.findByParentFolderId("folder-002");
        for (const auto& note : notes) {
            std::cout << "   - " << note.getName() << " (path: " << note.getPath() << ")" << std::endl;
        }
        std::cout << std::endl;

        // Search notes
        std::cout << "   Searching for notes containing 'BERT':" << std::endl;
        auto searchResults = noteDS.search("BERT");
        for (const auto& note : searchResults) {
            std::cout << "   - " << note.getName() << std::endl;
        }
        std::cout << std::endl;

        // ================================================================
        // 7. Update operations
        // ================================================================
        std::cout << "7. Updating data..." << std::endl;
        
        retrievedProject->setDescription("Updated: Advanced AI Research");
        projectDS.update(*retrievedProject);
        std::cout << "   ✓ Updated project description" << std::endl << std::endl;

        // ================================================================
        // 8. Check metrics
        // ================================================================
        std::cout << "8. Datasource metrics:" << std::endl;
        
        auto projectMetrics = projectDS.getMetrics();
        std::cout << "   Project DS:" << std::endl;
        std::cout << "      Total requests: " << projectMetrics.totalRequests << std::endl;
        std::cout << "      Success rate: " << projectMetrics.getSuccessRate() << "%" << std::endl;
        std::cout << "      Avg response time: " << projectMetrics.averageResponseTimeMs << "ms" << std::endl;
        
        auto folderMetrics = folderDS.getMetrics();
        std::cout << "   Folder DS:" << std::endl;
        std::cout << "      Total requests: " << folderMetrics.totalRequests << std::endl;
        std::cout << "      Success rate: " << folderMetrics.getSuccessRate() << "%" << std::endl;
        
        auto noteMetrics = noteDS.getMetrics();
        std::cout << "   Note DS:" << std::endl;
        std::cout << "      Total requests: " << noteMetrics.totalRequests << std::endl;
        std::cout << "      Success rate: " << noteMetrics.getSuccessRate() << "%" << std::endl << std::endl;

        // ================================================================
        // 9. Demonstrate CASCADE delete (relational integrity)
        // ================================================================
        std::cout << "9. Demonstrating CASCADE delete..." << std::endl;
        
        // Delete folder should cascade to its notes and subfolders
        std::cout << "   Deleting 'Papers' folder (should cascade to subfolders and notes)..." << std::endl;
        bool deleted = folderDS.deleteById("folder-001");
        std::cout << "   " << (deleted ? "✓" : "✗") << " Folder deleted" << std::endl;
        
        // Verify cascade
        auto remainingNotes = noteDS.findByParentFolderId("folder-002");
        std::cout << "   Notes remaining in deleted subfolder: " << remainingNotes.size() 
                  << " (should be 0 due to CASCADE)" << std::endl << std::endl;

        // ================================================================
        // 10. Final state
        // ================================================================
        std::cout << "10. Final database state:" << std::endl;
        
        auto allProjects = projectDS.findAll();
        auto allFolders = folderDS.findAll();
        auto allNotes = noteDS.findAll();
        
        std::cout << "   Total projects: " << allProjects.size() << std::endl;
        std::cout << "   Total folders: " << allFolders.size() << std::endl;
        std::cout << "   Total notes: " << allNotes.size() << std::endl << std::endl;

        // ================================================================
        // Cleanup
        // ================================================================
        projectDS.disconnect();
        folderDS.disconnect();
        noteDS.disconnect();
        
        std::cout << "=== Demo completed successfully! ===" << std::endl;
        std::cout << "\nDatabase file: " << dbPath << std::endl;
        std::cout << "Note storage: " << notesDir << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
