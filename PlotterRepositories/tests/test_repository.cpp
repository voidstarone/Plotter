#include <iostream>
#include <cassert>
#include "plotter_repositories/PlotterRepositories.h"
#include "Project.h"

// Include mock implementations
#include "mocks/MockProjectDataSource.h"
#include "mocks/MockRoutingStrategies.h"
#include "mocks/MockDataSourceRouter.h"

using namespace plotter::repositories;

void testBasicOperations() {
    std::cout << "Testing basic repository operations..." << std::endl;
    
    MockProjectDataSource datasource("test-db", "Memory", 100);
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&datasource);
    
    MultiSourceProjectRepository repo(&router);
    
    // Test save
    Project project("test-1", "Test Project", "Description");
    std::string id = repo.save(project);
    assert(id == "test-1");
    std::cout << "  ✓ Save operation" << std::endl;
    
    // Test findById
    auto found = repo.findById("test-1");
    assert(found.has_value());
    assert(found->getName() == "Test Project");
    std::cout << "  ✓ FindById operation" << std::endl;
    
    // Test exists
    assert(repo.exists("test-1") == true);
    assert(repo.exists("non-existent") == false);
    std::cout << "  ✓ Exists operation" << std::endl;
    
    // Test update
    Project updated("test-1", "Updated Project", "New description");
    repo.update(updated);
    auto foundUpdated = repo.findById("test-1");
    assert(foundUpdated->getName() == "Updated Project");
    std::cout << "  ✓ Update operation" << std::endl;
    
    // Test findAll
    repo.save(Project("test-2", "Project 2", "Desc 2"));
    auto all = repo.findAll();
    assert(all.size() == 2);
    std::cout << "  ✓ FindAll operation" << std::endl;
    
    // Test delete
    bool deleted = repo.deleteById("test-1");
    assert(deleted == true);
    assert(repo.exists("test-1") == false);
    std::cout << "  ✓ Delete operation" << std::endl;
}

void testMultiSourceWrite() {
    std::cout << "\nTesting multi-source write operations..." << std::endl;
    
    MockProjectDataSource db("database", "Database", 100);
    MockProjectDataSource cache("cache", "Memory", 200);
    
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&db);
    router.addDataSource(&cache);
    
    MultiSourceProjectRepository repo(&router);
    
    // Save should write to both
    Project project("multi-1", "Multi Source", "Test");
    repo.save(project);
    
    assert(db.getStorageSize() == 1);
    assert(cache.getStorageSize() == 1);
    std::cout << "  ✓ Write to multiple datasources" << std::endl;
    
    // Both should have the same data
    auto fromDb = db.findById("multi-1");
    auto fromCache = cache.findById("multi-1");
    assert(fromDb.has_value());
    assert(fromCache.has_value());
    assert(fromDb->getName() == fromCache->getName());
    std::cout << "  ✓ Data consistency across sources" << std::endl;
}

void testFailoverBehavior() {
    std::cout << "\nTesting failover behavior..." << std::endl;
    
    MockProjectDataSource primary("primary", "Database", 200);
    MockProjectDataSource secondary("secondary", "Database", 100);
    
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&primary);
    router.addDataSource(&secondary);
    
    MultiSourceProjectRepository repo(&router);
    
    // Save to both
    Project project("failover-1", "Failover Test", "Test");
    repo.save(project);
    
    // Disable primary
    primary.setAvailable(false);
    
    // Should still be able to read from secondary
    auto found = repo.findById("failover-1");
    assert(found.has_value());
    assert(found->getName() == "Failover Test");
    std::cout << "  ✓ Automatic failover on read" << std::endl;
    
    // Should still be able to write to secondary
    Project newProject("failover-2", "New Project", "During failover");
    repo.save(newProject);
    assert(secondary.exists("failover-2"));
    std::cout << "  ✓ Write during primary failure" << std::endl;
    
    // Re-enable primary
    primary.setAvailable(true);
    
    // Should work normally
    auto foundAgain = repo.findById("failover-1");
    assert(foundAgain.has_value());
    std::cout << "  ✓ Recovery after primary restoration" << std::endl;
}

void testErrorHandling() {
    std::cout << "\nTesting error handling..." << std::endl;
    
    MockProjectDataSource datasource("test-db", "Memory", 100);
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&datasource);
    
    MultiSourceProjectRepository repo(&router);
    
    // Save a project
    Project project("error-test", "Error Test", "Test");
    repo.save(project);
    
    // Disable datasource
    datasource.setAvailable(false);
    
    // Operations should throw with descriptive errors
    try {
        repo.save(Project("should-fail", "Fail", "Fail"));
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        assert(error.find("MultiSourceProjectRepository::save") != std::string::npos);
        std::cout << "  ✓ Save error with context" << std::endl;
    }
    
    try {
        repo.findById("error-test");
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        assert(error.find("MultiSourceProjectRepository::findById") != std::string::npos);
        std::cout << "  ✓ Read error with context" << std::endl;
    }
    
    // exists() should return false instead of throwing
    bool exists = repo.exists("error-test");
    assert(exists == false);
    std::cout << "  ✓ Exists returns false on error" << std::endl;
}

void testHealthMonitoring() {
    std::cout << "\nTesting health monitoring..." << std::endl;
    
    MockProjectDataSource ds1("source-1", "Database", 100);
    MockProjectDataSource ds2("source-2", "Memory", 200);
    
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&ds1);
    router.addDataSource(&ds2);
    
    // Check health of all sources
    auto healthResults = router.checkAllHealth();
    assert(healthResults.size() == 2);
    
    for (const auto& [name, health] : healthResults) {
        assert(health.isHealthy());
        assert(health.status == HealthStatus::HEALTHY);
    }
    std::cout << "  ✓ Health check for all datasources" << std::endl;
    
    // Disable one source
    ds1.setAvailable(false);
    healthResults = router.checkAllHealth();
    
    bool foundUnhealthy = false;
    for (const auto& [name, health] : healthResults) {
        if (name == "source-1") {
            assert(!health.isHealthy());
            assert(health.status == HealthStatus::UNHEALTHY);
            foundUnhealthy = true;
        }
    }
    assert(foundUnhealthy);
    std::cout << "  ✓ Unhealthy source detection" << std::endl;
}

void testMetricsTracking() {
    std::cout << "\nTesting metrics tracking..." << std::endl;
    
    MockProjectDataSource datasource("metrics-test", "Memory", 100);
    SimplePriorityStrategy strategy;
    SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
    router.addDataSource(&datasource);
    
    MultiSourceProjectRepository repo(&router);
    
    // Perform operations
    for (int i = 0; i < 5; i++) {
        Project p("proj-" + std::to_string(i), "Project " + std::to_string(i), "Desc");
        repo.save(p);
    }
    
    for (int i = 0; i < 5; i++) {
        repo.findById("proj-" + std::to_string(i));
    }
    
    auto metrics = datasource.getMetrics();
    assert(metrics.totalRequests == 10); // 5 saves + 5 finds
    assert(metrics.successfulRequests == 10);
    assert(metrics.getSuccessRate() == 100.0);
    
    std::cout << "  ✓ Request counting" << std::endl;
    std::cout << "  ✓ Success rate calculation: " << metrics.getSuccessRate() << "%" << std::endl;
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   PlotterRepositories - Unit Tests            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝\n" << std::endl;
    
    try {
        testBasicOperations();
        testMultiSourceWrite();
        testFailoverBehavior();
        testErrorHandling();
        testHealthMonitoring();
        testMetricsTracking();
        
        std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║   ✓ ALL TESTS PASSED                          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
}
