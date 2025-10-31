# Mock Implementations

This directory contains mock implementations of the PlotterRepositories interfaces for testing and demonstration purposes.

## Mock DataSources

### MockProjectDataSource
In-memory implementation of `ProjectDataSource` interface.
- Stores projects in a `std::map`
- Tracks metrics (request count, success rate, response time)
- Supports availability simulation (can be enabled/disabled for testing)
- Perfect for unit testing without external dependencies

### MockFolderDataSource
In-memory implementation of `FolderDataSource` interface.
- Stores folders with parent project/folder relationships
- Supports hierarchical queries
- Same metrics and availability features as ProjectDataSource

### MockNoteDataSource
In-memory implementation of `NoteDataSource` interface.
- Stores notes with folder and project associations
- Supports filtering by folder ID or project ID
- Full metrics tracking

## Mock Routing Strategies

### SimplePriorityStrategy
Basic implementation of priority-based routing.
- Selects datasource with highest priority for reads
- Writes to all available datasources
- Simple and predictable for testing

### SimpleCacheFirstStrategy
Cache-first routing implementation.
- Configurable cache type names
- Reads from cache first, falls back to other sources
- Write-through mode (writes to both cache and persistent storage)

## Mock Router

### SimpleDataSourceRouter<T>
Template-based router implementation.
- Manages collection of datasources
- Delegates routing decisions to strategy
- Automatic failover on errors
- Comprehensive error handling with context
- Health monitoring across all datasources

## Usage in Tests

```cpp
// Create mock datasources
MockProjectDataSource database("db", "Database", 100);
MockProjectDataSource cache("cache", "Memory", 200);

// Create router with strategy
SimplePriorityStrategy strategy;
SimpleDataSourceRouter<ProjectDataSource> router(&strategy);
router.addDataSource(&database);
router.addDataSource(&cache);

// Create repository
MultiSourceProjectRepository repository(&router);

// Use repository with mock infrastructure
Project project("id", "name", "description");
repository.save(project);
```

## Testing Features

### Availability Simulation
```cpp
MockProjectDataSource ds("test", "Memory", 100);
ds.setAvailable(false); // Simulate datasource failure
// Operations will now throw errors
ds.setAvailable(true); // Restore datasource
```

### Metrics Inspection
```cpp
auto metrics = datasource.getMetrics();
std::cout << "Total Requests: " << metrics.totalRequests << std::endl;
std::cout << "Success Rate: " << metrics.getSuccessRate() << "%" << std::endl;
std::cout << "Avg Response Time: " << metrics.averageResponseTimeMs << "ms" << std::endl;
```

### Health Checks
```cpp
auto health = datasource.checkHealth();
if (health.isHealthy()) {
    std::cout << "Datasource is operational" << std::endl;
}
```

## Benefits for Testing

1. **No External Dependencies**: Tests run without databases, caches, or file systems
2. **Fast Execution**: In-memory operations are extremely fast
3. **Predictable Behavior**: Controlled environment for deterministic tests
4. **Failure Simulation**: Easy to test error handling and failover scenarios
5. **Metrics Validation**: Verify system behavior through tracked metrics

## Examples

See:
- `../test_repository.cpp` - Comprehensive unit tests
- `../examples/demo_multi_source.cpp` - Full feature demonstration

## Creating Custom Mocks

To create mocks for your own datasources:

1. Inherit from the appropriate DataSource interface
2. Implement all pure virtual methods
3. Add test helpers (e.g., `setAvailable()`, `getStorageSize()`)
4. Track metrics for request counting and performance measurement
5. Use `std::map` or similar for simple in-memory storage

Example skeleton:
```cpp
class MockCustomDataSource : public CustomDataSource {
private:
    std::string name;
    int priority;
    bool available{true};
    std::map<std::string, CustomEntity> storage;
    DataSourceMetrics metrics;
    
public:
    // Implement interface methods...
    // Add test helpers...
};
```
