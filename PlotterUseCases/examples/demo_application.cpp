#include <iostream>
#include <memory>
#include <string>

// Entity includes
#include "Project.h"
#include "Folder.h" 
#include "Note.h"
#include "FileSystemNoteStorage.h"

// Repository interfaces
#include "repositories/ProjectRepository.h"
#include "repositories/FolderRepository.h"
#include "repositories/NoteRepository.h"

// Test mock implementations (in real app, these would come from infrastructure layer)
#include "InMemoryProjectRepository.h"
#include "InMemoryFolderRepository.h"
#include "InMemoryNoteRepository.h"

// Use cases
#include "usecases/CreateProjectUseCase.h"
#include "usecases/GetProjectUseCase.h"
#include "usecases/ListProjectsUseCase.h"
#include "usecases/CreateFolderUseCase.h"
#include "usecases/CreateNoteUseCase.h"
#include "usecases/GetNoteContentUseCase.h"
#include "usecases/MoveNoteUseCase.h"
#include "usecases/DeleteProjectUseCase.h"

/**
 * @brief Demo application showing clean architecture use cases in action
 * 
 * This application demonstrates:
 * 1. Dependency injection of repository implementations
 * 2. Use case execution with proper error handling
 * 3. Clean separation between application logic and infrastructure
 * 
 * NOTE: This demo uses in-memory repositories for simplicity.
 * In a real application, repository implementations would come from
 * the infrastructure layer (database, web service, etc.)
 */
class NoteTakerDemo {
private:
    // Infrastructure layer (normally injected from outside)
    std::shared_ptr<NoteStorage> noteStorage;
    
    // Repository implementations (normally from infrastructure layer)
    std::shared_ptr<ProjectRepository> projectRepo;
    std::shared_ptr<FolderRepository> folderRepo;
    std::shared_ptr<NoteRepository> noteRepo;
    
    // Use cases (application layer)
    std::shared_ptr<CreateProjectUseCase> createProject;
    std::shared_ptr<GetProjectUseCase> getProject;
    std::shared_ptr<ListProjectsUseCase> listProjects;
    std::shared_ptr<CreateFolderUseCase> createFolder;
    std::shared_ptr<CreateNoteUseCase> createNote;
    std::shared_ptr<GetNoteContentUseCase> getNoteContent;
    std::shared_ptr<MoveNoteUseCase> moveNote;
    std::shared_ptr<DeleteProjectUseCase> deleteProject;

public:
    NoteTakerDemo() {
        setupDependencies();
    }
    
    void run() {
        std::cout << "=== NoteTaker Use Cases Demo ===" << std::endl;
        std::cout << "Demonstrating Clean Architecture patterns" << std::endl << std::endl;
        
        try {
            // Demo workflow
            std::string projectId = createSampleProject();
            std::string folderId = createSampleFolder(projectId);
            std::string noteId = createSampleNote(folderId);
            
            listAllProjects();
            displayNoteContent(noteId);
            
            // Move note demo
            std::string newFolderId = createAnotherFolder(projectId);
            moveNoteBetweenFolders(noteId, newFolderId);
            
            // Cleanup demo
            cleanupDemo(projectId);
            
        } catch (const std::exception& e) {
            std::cerr << "Demo failed: " << e.what() << std::endl;
        }
        
        std::cout << std::endl << "=== Demo Complete ===" << std::endl;
    }

private:
    void setupDependencies() {
        std::cout << "Setting up dependencies (Dependency Injection)..." << std::endl;
        
        // Infrastructure layer
        noteStorage = std::make_shared<FileSystemNoteStorage>("./demo_notes");
        
        // Repository implementations (normally from DI container)
        projectRepo = std::make_shared<InMemoryProjectRepository>();
        folderRepo = std::make_shared<InMemoryFolderRepository>();
        noteRepo = std::make_shared<InMemoryNoteRepository>(noteStorage);
        
        // Use cases (application layer)
        createProject = std::make_shared<CreateProjectUseCase>(projectRepo);
        getProject = std::make_shared<GetProjectUseCase>(projectRepo);
        listProjects = std::make_shared<ListProjectsUseCase>(projectRepo);
        createFolder = std::make_shared<CreateFolderUseCase>(folderRepo, projectRepo);
        createNote = std::make_shared<CreateNoteUseCase>(noteRepo, folderRepo);
        getNoteContent = std::make_shared<GetNoteContentUseCase>(noteRepo);
        moveNote = std::make_shared<MoveNoteUseCase>(noteRepo, folderRepo);
        deleteProject = std::make_shared<DeleteProjectUseCase>(projectRepo, folderRepo, noteRepo);
        
        std::cout << "Dependencies configured successfully!" << std::endl << std::endl;
    }
    
    std::string createSampleProject() {
        std::cout << "1. Creating a sample project..." << std::endl;
        
        CreateProjectUseCase::Request request;
        request.name = "My Research Project";
        request.description = "A project for organizing research notes and ideas";
        
        auto response = createProject->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Project created with ID: " << response.projectId << std::endl;
            return response.projectId;
        } else {
            throw std::runtime_error("Failed to create project: " + response.errorMessage);
        }
    }
    
    std::string createSampleFolder(const std::string& projectId) {
        std::cout << "2. Creating a folder in the project..." << std::endl;
        
        CreateFolderUseCase::Request request;
        request.name = "Literature Review";
        request.description = "Folder for research papers and summaries";
        request.parentProjectId = projectId;
        
        auto response = createFolder->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Folder created with ID: " << response.folderId << std::endl;
            return response.folderId;
        } else {
            throw std::runtime_error("Failed to create folder: " + response.errorMessage);
        }
    }
    
    std::string createSampleNote(const std::string& folderId) {
        std::cout << "3. Creating a note in the folder..." << std::endl;
        
        CreateNoteUseCase::Request request;
        request.name = "AI Ethics Paper Notes";
        request.parentFolderId = folderId;
        request.initialContent = "# AI Ethics Research Notes\\n\\nKey points:\\n- Algorithmic bias\\n- Privacy concerns\\n- Transparency requirements";
        
        auto response = createNote->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Note created with ID: " << response.noteId << std::endl;
            std::cout << "   ✓ Note path: " << response.notePath << std::endl;
            return response.noteId;
        } else {
            throw std::runtime_error("Failed to create note: " + response.errorMessage);
        }
    }
    
    void listAllProjects() {
        std::cout << "4. Listing all projects..." << std::endl;
        
        ListProjectsUseCase::Request request;
        auto response = listProjects->execute(request);
        
        if (response.success) {
            std::cout << "   Found " << response.projects.size() << " project(s):" << std::endl;
            for (const auto& project : response.projects) {
                std::cout << "   - " << project.getName() << " (" << project.getId() << ")" << std::endl;
                std::cout << "     Description: " << project.getDescription() << std::endl;
                std::cout << "     Folders: " << project.getFolderIds().size() << std::endl;
            }
        } else {
            std::cout << "   Failed to list projects: " << response.errorMessage << std::endl;
        }
    }
    
    void displayNoteContent(const std::string& noteId) {
        std::cout << "5. Retrieving note content..." << std::endl;
        
        GetNoteContentUseCase::Request request;
        request.noteId = noteId;
        
        auto response = getNoteContent->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Note: " << response.noteName << std::endl;
            std::cout << "   ✓ Path: " << response.notePath << std::endl;
            std::cout << "   ✓ Content:" << std::endl;
            std::cout << "   " << response.content << std::endl;
        } else {
            std::cout << "   Failed to get note content: " << response.errorMessage << std::endl;
        }
    }
    
    std::string createAnotherFolder(const std::string& projectId) {
        std::cout << "6. Creating another folder for move demo..." << std::endl;
        
        CreateFolderUseCase::Request request;
        request.name = "Implementation Notes";
        request.description = "Technical implementation details";
        request.parentProjectId = projectId;
        
        auto response = createFolder->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Second folder created with ID: " << response.folderId << std::endl;
            return response.folderId;
        } else {
            throw std::runtime_error("Failed to create second folder: " + response.errorMessage);
        }
    }
    
    void moveNoteBetweenFolders(const std::string& noteId, const std::string& targetFolderId) {
        std::cout << "7. Moving note to different folder..." << std::endl;
        
        MoveNoteUseCase::Request request;
        request.noteId = noteId;
        request.targetFolderId = targetFolderId;
        
        auto response = moveNote->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Note moved successfully!" << std::endl;
        } else {
            std::cout << "   Failed to move note: " << response.errorMessage << std::endl;
        }
    }
    
    void cleanupDemo(const std::string& projectId) {
        std::cout << "8. Cleaning up (deleting project and all contents)..." << std::endl;
        
        DeleteProjectUseCase::Request request;
        request.projectId = projectId;
        
        auto response = deleteProject->execute(request);
        
        if (response.success) {
            std::cout << "   ✓ Project deleted successfully!" << std::endl;
            std::cout << "   ✓ Deleted " << response.deletedFolders << " folders" << std::endl;
            std::cout << "   ✓ Deleted " << response.deletedNotes << " notes" << std::endl;
        } else {
            std::cout << "   Failed to delete project: " << response.errorMessage << std::endl;
        }
    }
};

int main() {
    NoteTakerDemo demo;
    demo.run();
    return 0;
}