# Plotter Integration Tests

This directory contains integration tests for the Plotter backend data sources.

## Overview

The integration tests verify that data is correctly stored and retrievable from both SQLite and Filesystem backends by:

1. **Creating test data** through the data source APIs
2. **Verifying data independently** using external tools (Python with sqlite3, filesystem access)
3. **Cleaning up** by moving test files to trash (or permanently deleting if send2trash is not available)

## Test Scripts

### `integration_test_sqlite.py`
Tests the SQLite backend by:
- Creating a test SQLite database
- Inserting test projects, folders, and notes
- Verifying data directly in the database using Python's `sqlite3` module
- Checking schema integrity
- Moving the test database to trash after completion

### `integration_test_filesystem.py`
Tests the Filesystem backend by:
- Creating test directory structure with `.plotter_*` metadata files
- Creating test projects, folders, and notes as actual files/directories
- Verifying metadata in JSON dotfiles
- Checking file content
- Moving the test directory to trash after completion

### `run_integration_tests.sh`
Master script that runs all integration tests and provides a summary.

## Requirements

### Required
- Python 3.x
- Standard Python libraries: `sqlite3`, `json`, `os`, `sys`, `pathlib`, `tempfile`, `shutil`

### Optional (Recommended)
- `send2trash` - For safely moving test files to trash instead of permanent deletion
  ```bash
  pip3 install send2trash
  ```

Without `send2trash`, test files will be permanently deleted instead of moved to trash.

## Running Tests

### Run all integration tests
```bash
./tests/run_integration_tests.sh
```

### Run only SQLite tests
```bash
./tests/run_integration_tests.sh --sqlite-only
```

### Run only Filesystem tests
```bash
./tests/run_integration_tests.sh --filesystem-only
```

### Run individual tests
```bash
python3 ./tests/integration_test_sqlite.py
python3 ./tests/integration_test_filesystem.py
```

## What Gets Tested

### SQLite Backend
�� Database schema verification  
✅ Project data persistence  
✅ Folder data persistence  
✅ Note data persistence  
✅ Note content editing and updates  
✅ Relationship integrity (parent-child links)  
✅ Timestamp storage and updates  
✅ Data retrieval correctness  
✅ Multiple edit operations  

### Filesystem Backend
✅ Directory structure creation  
✅ `.plotter_project` metadata files  
✅ `.plotter_folder` metadata files  
✅ `.plotter_meta` note metadata files  
✅ Note content file storage  
✅ Note content editing and updates  
✅ Metadata timestamp updates on edit  
✅ JSON metadata format correctness  
✅ UUID generation and storage  
✅ Timestamp storage  
✅ Parent-child relationship tracking  
✅ Multiple edit operations

## Test Data Cleanup

All test files are automatically cleaned up after test execution:

- **With send2trash**: Files are moved to the system trash/recycle bin and can be recovered
- **Without send2trash**: Files are permanently deleted

Test files are created in the system's temporary directory:
- macOS/Linux: `/tmp/plotter_*`
- The exact paths are displayed during test execution

## Integration with CI/CD

These tests can be integrated into continuous integration pipelines:

```bash
# In your CI script
cd /path/to/Plotter
./tests/run_integration_tests.sh

# Exit code 0 = all tests passed
# Exit code 1 = some tests failed
```

## Extending the Tests

To add new integration tests:

1. Create a new Python script in the `tests/` directory
2. Follow the same pattern:
   - Create test data
   - Verify independently (not using Plotter code)
   - Clean up with `send2trash` or deletion
3. Add the test to `run_integration_tests.sh`

## Example Output

```
================================================
Plotter Backend Integration Tests
================================================

✓ Python 3 found
✓ send2trash installed (will use trash)

Running SQLite integration test...

============================================================
SQLite Data Source Integration Test
============================================================

ℹ Test database: /tmp/plotter_test_20241031_143022.db
✓ Test database created with sample data
✓ Schema verified
✓ Project verified in database
✓ Folder verified in database
✓ Note verified in database
✓ All SQLite integration tests passed!
ℹ Moved to trash: /tmp/plotter_test_20241031_143022.db

Running Filesystem integration test...

============================================================
Filesystem Data Source Integration Test
============================================================

ℹ Test directory: /tmp/plotter_fs_test_20241031_143023
ℹ Creating test filesystem structure...
✓ Test structure created
✓ Project verified: test-project-20241031143023
✓ Folder verified: test-folder-20241031143023
✓ Note verified: test-note-20241031143023
✓ All Filesystem integration tests passed!
ℹ Moved to trash: /tmp/plotter_fs_test_20241031_143023

================================================
Integration Test Summary
================================================

Total tests:  2
Passed:       2
Failed:       0

✅ All integration tests passed!
```

## Troubleshooting

### "send2trash not installed"
This is a warning, not an error. Tests will still run but files will be permanently deleted instead of moved to trash. Install with:
```bash
pip3 install send2trash
```

### "Python 3 not found"
Install Python 3:
- macOS: `brew install python3`
- Ubuntu: `sudo apt-get install python3`

### Permission errors
Ensure the test scripts are executable:
```bash
chmod +x tests/*.sh tests/*.py
```

## Notes

- Tests use temporary directories and do not affect your actual Plotter data
- Each test run uses unique timestamps to avoid conflicts
- Tests are designed to be idempotent and can be run multiple times
- The tests verify data **independently** from the Plotter codebase to ensure true integration testing
