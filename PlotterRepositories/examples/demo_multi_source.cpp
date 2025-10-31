#include <iostream>
#include <memory>
#include "plotter_repositories/PlotterRepositories.h"
#include "Project.h"
#include "Folder.h"
#include "Note.h"

// Include mock implementations
#include "mocks/MockProjectDataSource.h"
#include "mocks/MockFolderDataSource.h"
#include "mocks/MockNoteDataSource.h"
#include "mocks/MockRoutingStrategies.h"
#include "mocks/MockDataSourceRouter.h"

using namespace plotter::repositories;

void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void demonstrateBasicUsage() {
    printSeparator("BASIC USAGE: Single DataSource");
    
    // Create a single mock datasource
    MockProjectDataSource database("primary-db", "Database", 100);
    
    // Create router with simple priority strategy
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&database);
    
    // Create repository using the router
    MultiSourceProjectRepository repository(&router);
    
    // Use the repository
    std::cout << "Creating a project..." << std::endl;
    Project project("proj-001", "My First Project", "A simple test project");
    
    std::string savedId = repository.save(project);
    std::cout << "✓ Project saved with ID: " << savedId << std::endl;
    
    // Retrieve the project
    auto found = repository.findById("proj-001");
    if (found) {
        std::cout << "✓ Project retrieved: " << found->getName() << std::endl;
        std::cout << "  Description: " << found->getDescription() << std::endl;
    }
    
    // Check if exists
    bool exists = repository.exists("proj-001");
    std::cout << "✓ Project exists: " << (exists ? "YES" : "NO") << std::endl;
}

void demonstrateMultiSourceWithCaching() {
    printSeparator("MULTI-SOURCE: Cache + Database");
    
    // Create multiple datasources
    MockProjectDataSource database("primary-db", "Database", 100);
    MockProjectDataSource cache("redis-cache", "Memory", 200); // Higher priority
    
    // Create router with cache-first strategy
    SimpleCacheFirstStrategy strategy;
    strategy.setCacheTypes({"Memory", "Cache"});
    strategy.setWriteThroughEnabled(true);
    
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&database);
    router.addDataSource(&cache);
    
    // Create repository
    MultiSourceProjectRepository repository(&router);
    
    std::cout << "Creating project with multi-source setup..." << std::endl;
    Project project("proj-002", "Cached Project", "This will be cached");
    
    // Save writes to both cache and database
    repository.save(project);
    std::cout << "✓ Project saved to both cache and database" << std::endl;
    std::cout << "  Database size: " << database.getStorageSize() << std::endl;
    std::cout << "  Cache size: " << cache.getStorageSize() << std::endl;
    
    // Read should come from cache (higher priority)
    auto found = repository.findById("proj-002");
    if (found) {
        std::cout << "✓ Project retrieved from cache: " << found->getName() << std::endl;
    }
    
    // Check metrics
    std::cout << "\nDataSource Metrics:" << std::endl;
    std::cout << "  Database - Requests: " << database.getMetrics().totalRequests 
              << ", Success Rate: " << database.getMetrics().getSuccessRate() << "%" << std::endl;
    std::cout << "  Cache - Requests: " << cache.getMetrics().totalRequests
              << ", Success Rate: " << cache.getMetrics().getSuccessRate() << "%" << std::endl;
}

void demonstrateFailover() {
    printSeparator("FAILOVER: Automatic Fallback");
    
    // Create datasources
    MockProjectDataSource primary("primary-db", "Database", 200);
    MockProjectDataSource backup("backup-db", "Database", 100);
    
    // Create router with priority strategy
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&primary);
    router.addDataSource(&backup);
    
    MultiSourceProjectRepository repository(&router);
    
    // Save to both
    Project project("proj-003", "Failover Test", "Testing automatic failover");
    repository.save(project);
    std::cout << "✓ Project saved to primary and backup" << std::endl;
    
    // Simulate primary database failure
    std::cout << "\nSimulating primary database failure..." << std::endl;
    primary.setAvailable(false);
    
    // Read should automatically failover to backup
    std::cout << "Attempting to read (should use backup)..." << std::endl;
    try {
        auto found = repository.findById("proj-003");
        if (found) {
            std::cout << "✓ Successfully retrieved from backup: " << found->getName() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Error: " << e.what() << std::endl;
    }
    
    // Restore primary
    std::cout << "\nRestoring primary database..." << std::endl;
    primary.setAvailable(true);
    
    auto found = repository.findById("proj-003");
    if (found) {
        std::cout << "✓ Primary restored, read successful: " << found->getName() << std::endl;
    }
}

void demonstrateHealthMonitoring() {
    printSeparator("HEALTH MONITORING");
    
    // Create datasources
    MockProjectDataSource db1("db-node-1", "Database", 100);
    MockProjectDataSource db2("db-node-2", "Database", 100);
    MockProjectDataSource cache("cache-node", "Memory", 200);
    
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&db1);
    router.addDataSource(&db2);
    router.addDataSource(&cache);
    
    // Perform health checks
    std::cout << "Performing health checks on all datasources..." << std::endl;
    auto healthResults = router.checkAllHealth();
    
    for (const auto& [name, health] : healthResults) {
        std::cout << "\n  DataSource: " << name << std::endl;
        std::cout << "    Status: " << (health.isHealthy() ? "HEALTHY" : "UNHEALTHY") << std::endl;
        std::cout << "    Available: " << (health.isAvailable() ? "YES" : "NO") << std::endl;
        std::cout << "    Message: " << health.message << std::endl;
        std::cout << "    Total Requests: " << health.metrics.totalRequests << std::endl;
        std::cout << "    Success Rate: " << health.metrics.getSuccessRate() << "%" << std::endl;
    }
    
    // Simulate a datasource failure
    std::cout << "\nSimulating db-node-2 failure..." << std::endl;
    db2.setAvailable(false);
    
    auto updatedHealth = router.checkAllHealth();
    for (const auto& [name, health] : updatedHealth) {
        if (name == "db-node-2") {
            std::cout << "  " << name << " status: " 
                     << (health.isHealthy() ? "HEALTHY" : "UNHEALTHY") << std::endl;
        }
    }
}

void demonstrateCompleteWorkflow() {
    printSeparator("COMPLETE WORKFLOW: Project → Folder → Note");
    
    // Set up multi-source infrastructure
    MockProjectDataSource projectDB("project-db", "Database", 100);
    MockProjectDataSource projectCache("project-cache", "Memory", 200);
    
    MockFolderDataSource folderDB("folder-db", "Database", 100);
    MockFolderDataSource folderCache("folder-cache", "Memory", 200);
    
    MockNoteDataSource noteDB("note-db", "Database", 100);
    MockNoteDataSource noteCache("note-cache", "Memory", 200);
    
    // Create routers
    SimpleCacheFirstStrategy strategy;
    
    SimpleDataSourceRouter<ProjectDataSource> projectRouter(&strategy);
    projectRouter.addDataSource(&projectDB);
    projectRouter.addDataSource(&projectCache);
    
    SimpleDataSourceRouter<FolderDataSource> folderRouter(&strategy);
    folderRouter.addDataSource(&folderDB);
    folderRouter.addDataSource(&folderCache);
    
    SimpleDataSourceRouter<NoteDataSource> noteRouter(&strategy);
    noteRouter.addDataSource(&noteDB);
    noteRouter.addDataSource(&noteCache);
    
    // Create repositories
    MultiSourceProjectRepository projectRepo(&projectRouter);
    MultiSourceFolderRepository folderRepo(&folderRouter);
    MultiSourceNoteRepository noteRepo(&noteRouter);
    
    // Create a project
    std::cout << "1. Creating a project..." << std::endl;
    Project project("proj-100", "Research Project", "AI and Machine Learning research");
    projectRepo.save(project);
    std::cout << "   ✓ Project created: " << project.getName() << std::endl;
    
    // Create a folder in the project
    std::cout << "\n2. Creating a folder in the project..." << std::endl;
    Folder folder("folder-100", "Literature Review", "proj-100");
    folderRepo.save(folder);
    std::cout << "   ✓ Folder created: " << folder.getName() << std::endl;
    
    // Create notes in the folder
    std::cout << "\n3. Creating notes in the folder..." << std::endl;
    Note note1("note-101", "Paper 1 Summary", "proj-100", "folder-100");
    Note note2("note-102", "Paper 2 Summary", "proj-100", "folder-100");
    Note note3("note-103", "Research Ideas", "proj-100", "folder-100");
    
    noteRepo.save(note1);
    noteRepo.save(note2);
    noteRepo.save(note3);
    std::cout << "   ✓ Created 3 notes" << std::endl;
    
    // Query the structure
    std::cout << "\n4. Querying the structure..." << std::endl;
    
    auto foundProject = projectRepo.findById("proj-100");
    if (foundProject) {
        std::cout << "   Project: " << foundProject->getName() << std::endl;
    }
    
    auto projectFolders = folderRepo.findByParentProjectId("proj-100");
    std::cout << "   Folders in project: " << projectFolders.size() << std::endl;
    
    auto folderNotes = noteRepo.findByFolderId("folder-100");
    std::cout << "   Notes in folder: " << folderNotes.size() << std::endl;
    for (const auto& note : folderNotes) {
        std::cout << "     - " << note.getName() << std::endl;
    }
    
    // Show storage distribution
    std::cout << "\n5. Storage distribution:" << std::endl;
    std::cout << "   Project DB: " << projectDB.getStorageSize() 
              << " | Cache: " << projectCache.getStorageSize() << std::endl;
    std::cout << "   Folder DB: " << folderDB.getStorageSize() 
              << " | Cache: " << folderCache.getStorageSize() << std::endl;
    std::cout << "   Note DB: " << noteDB.getStorageSize() 
              << " | Cache: " << noteCache.getStorageSize() << std::endl;
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   PlotterRepositories - Multi-Source Repository Demo      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    try {
        demonstrateBasicUsage();
        demonstrateMultiSourceWithCaching();
        demonstrateFailover();
        demonstrateHealthMonitoring();
        demonstrateCompleteWorkflow();
        
        printSeparator("ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY");
        std::cout << "\n✓ All features working correctly!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error occurred: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
