#include <iostream>
#include <memory>
#include "plotter_repositories/PlotterRepositories.h"
#include "../tests/mocks/MockProjectDataSource.h"
#include "../tests/mocks/MockDataSourceRouter.h"
#include "../tests/mocks/MockRoutingStrategies.h"
#include "plotter_repositories/MultiSourceProjectRepository.h"

using namespace plotter::repositories;

int main() {
    std::cout << "=== Simple PlotterRepositories Demo ===" << std::endl << std::endl;
    
    // Create mock datasources
    MockProjectDataSource database("database", "PostgreSQL", 100);
    MockProjectDataSource cache("cache", "Redis", 200);
    
    // Create routing strategy
    SimplePriorityStrategy strategy;
    
    // Create router (concrete implementation from routing package/mocks)
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&database);
    router.addDataSource(&cache);
    
    // Create repository with concrete router type
    MultiSourceProjectRepository<SimpleDataSourceRouter<ProjectDataSource>> repo(&router);
    
    std::cout << "1. Creating a project..." << std::endl;
    Project project("proj-1", "My Research", "A research project on AI");
    std::string savedId = repo.save(project);
    std::cout << "   Saved project with ID: " << savedId << std::endl << std::endl;
    
    std::cout << "2. Retrieving the project..." << std::endl;
    auto retrieved = repo.findById("proj-1");
    if (retrieved) {
        std::cout << "   Found: " << retrieved->getName() << std::endl;
        std::cout << "   Description: " << retrieved->getDescription() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "3. Checking if project exists..." << std::endl;
    bool exists = repo.exists("proj-1");
    std::cout << "   Project exists: " << (exists ? "Yes" : "No") << std::endl << std::endl;
    
    std::cout << "4. Updating the project..." << std::endl;
    Project updated("proj-1", "My Research (Updated)", "Updated description");
    repo.update(updated);
    auto updatedProject = repo.findById("proj-1");
    if (updatedProject) {
        std::cout << "   Updated name: " << updatedProject->getName() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "5. Checking datasource metrics..." << std::endl;
    auto dbMetrics = database.getMetrics();
    auto cacheMetrics = cache.getMetrics();
    std::cout << "   Database - Requests: " << dbMetrics.totalRequests 
              << ", Success rate: " << dbMetrics.getSuccessRate() << "%" << std::endl;
    std::cout << "   Cache - Requests: " << cacheMetrics.totalRequests 
              << ", Success rate: " << cacheMetrics.getSuccessRate() << "%" << std::endl;
    std::cout << std::endl;
    
    std::cout << "6. Deleting the project..." << std::endl;
    bool deleted = repo.deleteById("proj-1");
    std::cout << "   Deleted: " << (deleted ? "Yes" : "No") << std::endl;
    exists = repo.exists("proj-1");
    std::cout << "   Still exists: " << (exists ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Demo completed successfully!" << std::endl;
    
    return 0;
}
