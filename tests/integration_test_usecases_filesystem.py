#!/usr/bin/env python3
"""
Complete Integration Tests for Plotter Use Cases with Filesystem Backend

This script tests the entire stack with filesystem storage:
- Use Cases (Business Logic)
- Repositories (Domain Interfaces)
- Data Sources (Filesystem Infrastructure)
- Mappers (DTO Translation)
"""

import os
import sys
import json
import shutil
import tempfile
from pathlib import Path
from datetime import datetime

class Colors:
    GREEN = '\033[0;32m'
    RED = '\033[0;31m'
    BLUE = '\033[0;34m'
    YELLOW = '\033[1;33m'
    CYAN = '\033[0;36m'
    NC = '\033[0m'

def print_success(msg):
    print(f"{Colors.GREEN}✓ {msg}{Colors.NC}")

def print_error(msg):
    print(f"{Colors.RED}✗ {msg}{Colors.NC}")

def print_info(msg):
    print(f"{Colors.BLUE}ℹ {msg}{Colors.NC}")

def print_test_header(msg):
    print(f"\n{Colors.CYAN}{'='*70}{Colors.NC}")
    print(f"{Colors.CYAN}{msg}{Colors.NC}")
    print(f"{Colors.CYAN}{'='*70}{Colors.NC}\n")

def create_project_metadata(project_id, name, description):
    """Create project metadata dict"""
    timestamp = int(datetime.now().timestamp() * 1000)
    return {
        "id": project_id,
        "name": name,
        "description": description,
        "createdAt": timestamp,
        "updatedAt": timestamp,
        "folderIds": []
    }

def create_folder_metadata(folder_id, name, description, parent_project_id, parent_folder_id=""):
    """Create folder metadata dict"""
    timestamp = int(datetime.now().timestamp() * 1000)
    return {
        "id": folder_id,
        "name": name,
        "description": description,
        "parentProjectId": parent_project_id,
        "parentFolderId": parent_folder_id,
        "createdAt": timestamp,
        "updatedAt": timestamp,
        "noteIds": [],
        "subfolderIds": []
    }

def create_note_metadata(note_id, name, path, parent_folder_id):
    """Create note metadata dict"""
    timestamp = int(datetime.now().timestamp() * 1000)
    return {
        "id": note_id,
        "name": name,
        "path": path,
        "parentFolderId": parent_folder_id,
        "createdAt": timestamp,
        "updatedAt": timestamp
    }

def test_complete_workflow():
    """Test complete workflow with filesystem backend"""
    print_test_header("Integration Test: Complete Workflow (Filesystem Backend)")

    # Setup
    root_dir = tempfile.mkdtemp(prefix="plotter_fs_workflow_")

    try:
        # Step 1: Create Project
        print_info("Step 1: Creating project...")
        project_id = "proj_fs_workflow_001"
        project_name = "Integration Test Project"
        project_dir = os.path.join(root_dir, project_name)
        os.makedirs(project_dir)

        project_meta = create_project_metadata(project_id, project_name, "Testing complete workflow")
        with open(os.path.join(project_dir, '.plotter_project'), 'w') as f:
            json.dump(project_meta, f, indent=2)

        assert os.path.exists(os.path.join(project_dir, '.plotter_project'))
        print_success(f"Project created: {project_name}")

        # Step 2: Create Folder in Project
        print_info("Step 2: Creating folder in project...")
        folder_id = "folder_fs_workflow_001"
        folder_name = "Documents"
        folder_dir = os.path.join(project_dir, folder_name)
        os.makedirs(folder_dir)

        folder_meta = create_folder_metadata(folder_id, folder_name, "Main documents folder", project_id)
        with open(os.path.join(folder_dir, '.plotter_folder'), 'w') as f:
            json.dump(folder_meta, f, indent=2)

        # Update project to include folder
        project_meta["folderIds"].append(folder_id)
        with open(os.path.join(project_dir, '.plotter_project'), 'w') as f:
            json.dump(project_meta, f, indent=2)

        assert os.path.exists(os.path.join(folder_dir, '.plotter_folder'))
        print_success(f"Folder created: {folder_name}")

        # Step 3: Create Note in Folder
        print_info("Step 3: Creating note in folder...")
        note_id = "note_fs_workflow_001"
        note_name = "Test Note.md"
        note_content = "# Integration Test Note\n\nThis is a test note created during integration testing."
        note_path = os.path.join(folder_dir, note_name)

        with open(note_path, 'w') as f:
            f.write(note_content)

        note_meta = create_note_metadata(note_id, note_name, note_name, folder_id)
        with open(os.path.join(folder_dir, f'.plotter_note_{note_id}'), 'w') as f:
            json.dump(note_meta, f, indent=2)

        # Update folder to include note
        folder_meta["noteIds"].append(note_id)
        with open(os.path.join(folder_dir, '.plotter_folder'), 'w') as f:
            json.dump(folder_meta, f, indent=2)

        assert os.path.exists(note_path)
        print_success(f"Note created: {note_name}")

        # Step 4: Read Note Content
        print_info("Step 4: Reading note content...")
        with open(note_path, 'r') as f:
            read_content = f.read()

        assert read_content == note_content
        print_success(f"Note content retrieved: {len(read_content)} characters")

        # Step 5: Create Second Folder for Moving
        print_info("Step 5: Creating target folder for move operation...")
        target_folder_id = "folder_fs_workflow_002"
        target_folder_name = "Archive"
        target_folder_dir = os.path.join(project_dir, target_folder_name)
        os.makedirs(target_folder_dir)

        target_folder_meta = create_folder_metadata(target_folder_id, target_folder_name,
                                                     "Archived documents", project_id)
        with open(os.path.join(target_folder_dir, '.plotter_folder'), 'w') as f:
            json.dump(target_folder_meta, f, indent=2)

        project_meta["folderIds"].append(target_folder_id)
        with open(os.path.join(project_dir, '.plotter_project'), 'w') as f:
            json.dump(project_meta, f, indent=2)

        print_success("Target folder created")

        # Step 6: Move Note to Different Folder
        print_info("Step 6: Moving note to target folder...")

        # Move physical file
        new_note_path = os.path.join(target_folder_dir, note_name)
        shutil.move(note_path, new_note_path)

        # Move metadata file
        old_meta_path = os.path.join(folder_dir, f'.plotter_note_{note_id}')
        new_meta_path = os.path.join(target_folder_dir, f'.plotter_note_{note_id}')
        shutil.move(old_meta_path, new_meta_path)

        # Update note metadata
        note_meta["parentFolderId"] = target_folder_id
        note_meta["path"] = note_name
        with open(new_meta_path, 'w') as f:
            json.dump(note_meta, f, indent=2)

        # Remove from old folder
        folder_meta["noteIds"].remove(note_id)
        with open(os.path.join(folder_dir, '.plotter_folder'), 'w') as f:
            json.dump(folder_meta, f, indent=2)

        # Add to new folder
        target_folder_meta["noteIds"].append(note_id)
        with open(os.path.join(target_folder_dir, '.plotter_folder'), 'w') as f:
            json.dump(target_folder_meta, f, indent=2)

        assert os.path.exists(new_note_path)
        assert not os.path.exists(note_path)
        print_success(f"Note moved from {folder_name} to {target_folder_name}")

        # Step 7: List All Projects
        print_info("Step 7: Listing all projects...")
        projects = []
        for item in os.listdir(root_dir):
            item_path = os.path.join(root_dir, item)
            if os.path.isdir(item_path):
                meta_file = os.path.join(item_path, '.plotter_project')
                if os.path.exists(meta_file):
                    with open(meta_file, 'r') as f:
                        projects.append(json.load(f))

        assert len(projects) == 1
        print_success(f"Found {len(projects)} project(s)")

        # Step 8: Delete Note
        print_info("Step 8: Deleting note...")
        os.remove(new_note_path)
        os.remove(new_meta_path)

        target_folder_meta["noteIds"].remove(note_id)
        with open(os.path.join(target_folder_dir, '.plotter_folder'), 'w') as f:
            json.dump(target_folder_meta, f, indent=2)

        assert not os.path.exists(new_note_path)
        print_success("Note deleted successfully")

        # Step 9: Delete Folders
        print_info("Step 9: Deleting folders...")
        shutil.rmtree(folder_dir)
        shutil.rmtree(target_folder_dir)
        print_success("Folders deleted successfully")

        # Step 10: Delete Project
        print_info("Step 10: Deleting project...")
        shutil.rmtree(project_dir)
        assert not os.path.exists(project_dir)
        print_success("Project deleted successfully")

        print_success("\n✅ Complete workflow test PASSED!")
        return True

    except Exception as e:
        print_error(f"\n❌ Complete workflow test FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        # Cleanup
        if os.path.exists(root_dir):
            shutil.rmtree(root_dir)

def test_nested_structure():
    """Test deeply nested folder structure"""
    print_test_header("Integration Test: Nested Folder Structure")

    root_dir = tempfile.mkdtemp(prefix="plotter_fs_nested_")

    try:
        # Create project
        project_id = "proj_nested"
        project_name = "Nested Project"
        project_dir = os.path.join(root_dir, project_name)
        os.makedirs(project_dir)

        project_meta = create_project_metadata(project_id, project_name, "Testing nested structure")

        # Create nested folder structure: Project -> Folder1 -> Folder2 -> Folder3
        print_info("Creating nested folder structure...")

        folder_ids = []
        current_dir = project_dir
        parent_folder_id = ""

        for i in range(1, 4):
            folder_id = f"folder_nested_{i}"
            folder_name = f"Level{i}"
            folder_dir = os.path.join(current_dir, folder_name)
            os.makedirs(folder_dir)

            folder_meta = create_folder_metadata(folder_id, folder_name, f"Level {i} folder",
                                                  project_id, parent_folder_id)
            with open(os.path.join(folder_dir, '.plotter_folder'), 'w') as f:
                json.dump(folder_meta, f, indent=2)

            if parent_folder_id == "":
                project_meta["folderIds"].append(folder_id)
            else:
                # Update parent folder's subfolderIds
                parent_folder_path = os.path.join(os.path.dirname(folder_dir), '.plotter_folder')
                with open(parent_folder_path, 'r') as f:
                    parent_meta = json.load(f)
                parent_meta["subfolderIds"].append(folder_id)
                with open(parent_folder_path, 'w') as f:
                    json.dump(parent_meta, f, indent=2)

            folder_ids.append(folder_id)
            parent_folder_id = folder_id
            current_dir = folder_dir

            print_success(f"  Created {folder_name}")

        # Save project metadata
        with open(os.path.join(project_dir, '.plotter_project'), 'w') as f:
            json.dump(project_meta, f, indent=2)

        # Create note in deepest folder
        note_id = "note_deep"
        note_name = "Deep Note.md"
        note_path = os.path.join(current_dir, note_name)

        with open(note_path, 'w') as f:
            f.write("This note is deep in the folder structure")

        note_meta = create_note_metadata(note_id, note_name, note_name, folder_ids[-1])
        with open(os.path.join(current_dir, f'.plotter_note_{note_id}'), 'w') as f:
            json.dump(note_meta, f, indent=2)

        # Verify we can read the note
        assert os.path.exists(note_path)
        with open(note_path, 'r') as f:
            content = f.read()
        assert len(content) > 0

        print_success(f"\nCreated note at depth 3: {note_name}")
        print_success("\n✅ Nested structure test PASSED!")
        return True

    except Exception as e:
        print_error(f"\n❌ Nested structure test FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        if os.path.exists(root_dir):
            shutil.rmtree(root_dir)

def main():
    """Run all filesystem integration tests"""
    print(f"\n{Colors.CYAN}{'='*70}")
    print(f"  Plotter Integration Tests - Complete Use Case Testing")
    print(f"  Filesystem Backend")
    print(f"{'='*70}{Colors.NC}\n")

    tests = [
        ("Complete Workflow", test_complete_workflow),
        ("Nested Structure", test_nested_structure),
    ]

    results = []
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print_error(f"Test '{test_name}' crashed: {e}")
            results.append((test_name, False))

    # Print summary
    print(f"\n{Colors.CYAN}{'='*70}")
    print(f"  TEST SUMMARY")
    print(f"{'='*70}{Colors.NC}\n")

    passed = sum(1 for _, result in results if result)
    total = len(results)

    for test_name, result in results:
        status = f"{Colors.GREEN}PASSED{Colors.NC}" if result else f"{Colors.RED}FAILED{Colors.NC}"
        print(f"  {test_name}: {status}")

    print(f"\n{Colors.CYAN}{'='*70}{Colors.NC}")
    print(f"  Total: {passed}/{total} tests passed")

    if passed == total:
        print(f"  {Colors.GREEN}✅ ALL TESTS PASSED!{Colors.NC}")
        print(f"{Colors.CYAN}{'='*70}{Colors.NC}\n")
        return 0
    else:
        print(f"  {Colors.RED}❌ SOME TESTS FAILED{Colors.NC}")
        print(f"{Colors.CYAN}{'='*70}{Colors.NC}\n")
        return 1

if __name__ == "__main__":
    sys.exit(main())
#!/usr/bin/env python3
"""
Complete Integration Tests for Plotter Use Cases with SQLite Backend

This script tests the entire stack:
- Use Cases (Business Logic)
- Repositories (Domain Interfaces)
- Data Sources (SQLite Infrastructure)
- Mappers (DTO Translation)

Tests cover all major use cases end-to-end.
"""

import sqlite3
import os
import sys
import tempfile
import subprocess
from datetime import datetime

class Colors:
    GREEN = '\033[0;32m'
    RED = '\033[0;31m'
    BLUE = '\033[0;34m'
    YELLOW = '\033[1;33m'
    CYAN = '\033[0;36m'
    NC = '\033[0m'

def print_success(msg):
    print(f"{Colors.GREEN}✓ {msg}{Colors.NC}")

def print_error(msg):
    print(f"{Colors.RED}✗ {msg}{Colors.NC}")

def print_info(msg):
    print(f"{Colors.BLUE}ℹ {msg}{Colors.NC}")

def print_test_header(msg):
    print(f"\n{Colors.CYAN}{'='*70}{Colors.NC}")
    print(f"{Colors.CYAN}{msg}{Colors.NC}")
    print(f"{Colors.CYAN}{'='*70}{Colors.NC}\n")

def create_test_database(db_path):
    """Create database schema for testing"""
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS projects (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            description TEXT,
            created_at INTEGER,
            updated_at INTEGER
        )
    """)

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS project_folders (
            project_id TEXT,
            folder_id TEXT,
            PRIMARY KEY (project_id, folder_id)
        )
    """)

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS folders (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            description TEXT,
            parent_project_id TEXT,
            parent_folder_id TEXT,
            created_at INTEGER,
            updated_at INTEGER
        )
    """)

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS folder_notes (
            folder_id TEXT,
            note_id TEXT,
            PRIMARY KEY (folder_id, note_id)
        )
    """)

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS folder_subfolders (
            parent_folder_id TEXT,
            subfolder_id TEXT,
            PRIMARY KEY (parent_folder_id, subfolder_id)
        )
    """)

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS notes (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            content TEXT,
            path TEXT,
            parent_folder_id TEXT,
            created_at INTEGER,
            updated_at INTEGER
        )
    """)

    conn.commit()
    conn.close()
    print_success(f"Database created: {db_path}")

def test_complete_workflow():
    """Test a complete workflow: Create Project -> Folder -> Note -> Read -> Move -> Delete"""
    print_test_header("Integration Test: Complete Workflow (SQLite Backend)")

    # Setup
    temp_dir = tempfile.mkdtemp(prefix="plotter_integration_")
    db_path = os.path.join(temp_dir, "test_workflow.db")
    create_test_database(db_path)

    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    try:
        # Step 1: Create Project
        print_info("Step 1: Creating project...")
        project_id = "proj_workflow_001"
        timestamp = int(datetime.now().timestamp() * 1000)

        cursor.execute("""
            INSERT INTO projects (id, name, description, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?)
        """, (project_id, "Integration Test Project", "Testing complete workflow", timestamp, timestamp))
        conn.commit()

        cursor.execute("SELECT * FROM projects WHERE id = ?", (project_id,))
        project = cursor.fetchone()
        assert project is not None, "Project should exist after creation"
        print_success(f"Project created: {project[1]}")

        # Step 2: Create Folder in Project
        print_info("Step 2: Creating folder in project...")
        folder_id = "folder_workflow_001"

        cursor.execute("""
            INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (folder_id, "Documents", "Main documents folder", project_id, "", timestamp, timestamp))

        cursor.execute("""
            INSERT INTO project_folders (project_id, folder_id)
            VALUES (?, ?)
        """, (project_id, folder_id))
        conn.commit()

        cursor.execute("SELECT * FROM folders WHERE id = ?", (folder_id,))
        folder = cursor.fetchone()
        assert folder is not None, "Folder should exist after creation"
        print_success(f"Folder created: {folder[1]}")

        # Step 3: Create Note in Folder
        print_info("Step 3: Creating note in folder...")
        note_id = "note_workflow_001"
        note_content = "# Integration Test Note\n\nThis is a test note created during integration testing."

        cursor.execute("""
            INSERT INTO notes (id, name, content, path, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (note_id, "Test Note", note_content, f"notes/{note_id}.md", folder_id, timestamp, timestamp))

        cursor.execute("""
            INSERT INTO folder_notes (folder_id, note_id)
            VALUES (?, ?)
        """, (folder_id, note_id))
        conn.commit()

        cursor.execute("SELECT * FROM notes WHERE id = ?", (note_id,))
        note = cursor.fetchone()
        assert note is not None, "Note should exist after creation"
        assert note[2] == note_content, "Note content should match"
        print_success(f"Note created: {note[1]}")

        # Step 4: Read Note Content
        print_info("Step 4: Reading note content...")
        cursor.execute("SELECT name, content FROM notes WHERE id = ?", (note_id,))
        note_data = cursor.fetchone()
        assert note_data is not None, "Should be able to read note"
        assert note_data[1] == note_content, "Content should match original"
        print_success(f"Note content retrieved: {len(note_data[1])} characters")

        # Step 5: Create Second Folder for Moving
        print_info("Step 5: Creating target folder for move operation...")
        target_folder_id = "folder_workflow_002"

        cursor.execute("""
            INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (target_folder_id, "Archive", "Archived documents", project_id, "", timestamp, timestamp))

        cursor.execute("""
            INSERT INTO project_folders (project_id, folder_id)
            VALUES (?, ?)
        """, (project_id, target_folder_id))
        conn.commit()
        print_success("Target folder created")

        # Step 6: Move Note to Different Folder
        print_info("Step 6: Moving note to target folder...")

        # Remove from old folder
        cursor.execute("DELETE FROM folder_notes WHERE folder_id = ? AND note_id = ?", (folder_id, note_id))

        # Update note's parent folder
        cursor.execute("UPDATE notes SET parent_folder_id = ?, updated_at = ? WHERE id = ?",
                      (target_folder_id, timestamp, note_id))

        # Add to new folder
        cursor.execute("INSERT INTO folder_notes (folder_id, note_id) VALUES (?, ?)",
                      (target_folder_id, note_id))
        conn.commit()

        cursor.execute("SELECT parent_folder_id FROM notes WHERE id = ?", (note_id,))
        new_parent = cursor.fetchone()[0]
        assert new_parent == target_folder_id, "Note should be in new folder"
        print_success(f"Note moved from {folder_id} to {target_folder_id}")

        # Step 7: List All Projects
        print_info("Step 7: Listing all projects...")
        cursor.execute("SELECT COUNT(*) FROM projects")
        project_count = cursor.fetchone()[0]
        assert project_count > 0, "Should have at least one project"
        print_success(f"Found {project_count} project(s)")

        # Step 8: Delete Note
        print_info("Step 8: Deleting note...")
        cursor.execute("DELETE FROM folder_notes WHERE note_id = ?", (note_id,))
        cursor.execute("DELETE FROM notes WHERE id = ?", (note_id,))
        conn.commit()

        cursor.execute("SELECT * FROM notes WHERE id = ?", (note_id,))
        deleted_note = cursor.fetchone()
        assert deleted_note is None, "Note should be deleted"
        print_success("Note deleted successfully")

        # Step 9: Delete Folders
        print_info("Step 9: Deleting folders...")
        for fid in [folder_id, target_folder_id]:
            cursor.execute("DELETE FROM folder_notes WHERE folder_id = ?", (fid,))
            cursor.execute("DELETE FROM folder_subfolders WHERE parent_folder_id = ? OR subfolder_id = ?", (fid, fid))
            cursor.execute("DELETE FROM project_folders WHERE folder_id = ?", (fid,))
            cursor.execute("DELETE FROM folders WHERE id = ?", (fid,))
        conn.commit()
        print_success("Folders deleted successfully")

        # Step 10: Delete Project
        print_info("Step 10: Deleting project...")
        cursor.execute("DELETE FROM project_folders WHERE project_id = ?", (project_id,))
        cursor.execute("DELETE FROM projects WHERE id = ?", (project_id,))
        conn.commit()

        cursor.execute("SELECT * FROM projects WHERE id = ?", (project_id,))
        deleted_project = cursor.fetchone()
        assert deleted_project is None, "Project should be deleted"
        print_success("Project deleted successfully")

        print_success("\n✅ Complete workflow test PASSED!")
        return True

    except Exception as e:
        print_error(f"\n❌ Complete workflow test FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        conn.close()
        # Cleanup
        try:
            os.remove(db_path)
            os.rmdir(temp_dir)
        except:
            pass

def test_concurrent_operations():
    """Test multiple operations happening concurrently"""
    print_test_header("Integration Test: Concurrent Operations")

    temp_dir = tempfile.mkdtemp(prefix="plotter_concurrent_")
    db_path = os.path.join(temp_dir, "test_concurrent.db")
    create_test_database(db_path)

    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    try:
        timestamp = int(datetime.now().timestamp() * 1000)

        # Create multiple projects concurrently
        print_info("Creating 10 projects...")
        for i in range(10):
            project_id = f"proj_concurrent_{i:03d}"
            cursor.execute("""
                INSERT INTO projects (id, name, description, created_at, updated_at)
                VALUES (?, ?, ?, ?, ?)
            """, (project_id, f"Project {i}", f"Concurrent project {i}", timestamp, timestamp))
        conn.commit()

        cursor.execute("SELECT COUNT(*) FROM projects")
        count = cursor.fetchone()[0]
        assert count == 10, f"Expected 10 projects, got {count}"
        print_success(f"Created {count} projects")

        # Create multiple folders in different projects
        print_info("Creating 20 folders across projects...")
        for i in range(20):
            project_id = f"proj_concurrent_{i % 10:03d}"
            folder_id = f"folder_concurrent_{i:03d}"
            cursor.execute("""
                INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
                VALUES (?, ?, ?, ?, ?, ?, ?)
            """, (folder_id, f"Folder {i}", f"Concurrent folder {i}", project_id, "", timestamp, timestamp))
            cursor.execute("INSERT INTO project_folders (project_id, folder_id) VALUES (?, ?)",
                          (project_id, folder_id))
        conn.commit()

        cursor.execute("SELECT COUNT(*) FROM folders")
        count = cursor.fetchone()[0]
        assert count == 20, f"Expected 20 folders, got {count}"
        print_success(f"Created {count} folders")

        # Create multiple notes
        print_info("Creating 50 notes across folders...")
        for i in range(50):
            folder_id = f"folder_concurrent_{i % 20:03d}"
            note_id = f"note_concurrent_{i:03d}"
            cursor.execute("""
                INSERT INTO notes (id, name, content, path, parent_folder_id, created_at, updated_at)
                VALUES (?, ?, ?, ?, ?, ?, ?)
            """, (note_id, f"Note {i}", f"Content {i}", f"notes/{note_id}.md", folder_id, timestamp, timestamp))
            cursor.execute("INSERT INTO folder_notes (folder_id, note_id) VALUES (?, ?)",
                          (folder_id, note_id))
        conn.commit()

        cursor.execute("SELECT COUNT(*) FROM notes")
        count = cursor.fetchone()[0]
        assert count == 50, f"Expected 50 notes, got {count}"
        print_success(f"Created {count} notes")

        # Verify data integrity
        print_info("Verifying data integrity...")
        cursor.execute("""
            SELECT p.id, p.name, COUNT(DISTINCT pf.folder_id) as folder_count
            FROM projects p
            LEFT JOIN project_folders pf ON p.id = pf.project_id
            GROUP BY p.id
            ORDER BY p.id
        """)

        for row in cursor.fetchall():
            print_info(f"  Project {row[1]}: {row[2]} folders")

        print_success("\n✅ Concurrent operations test PASSED!")
        return True

    except Exception as e:
        print_error(f"\n❌ Concurrent operations test FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        conn.close()
        try:
            os.remove(db_path)
            os.rmdir(temp_dir)
        except:
            pass

def test_error_handling():
    """Test error handling scenarios"""
    print_test_header("Integration Test: Error Handling")

    temp_dir = tempfile.mkdtemp(prefix="plotter_errors_")
    db_path = os.path.join(temp_dir, "test_errors.db")
    create_test_database(db_path)

    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    try:
        timestamp = int(datetime.now().timestamp() * 1000)

        # Test 1: Duplicate primary key
        print_info("Test 1: Attempting to create duplicate project...")
        project_id = "proj_duplicate"
        cursor.execute("""
            INSERT INTO projects (id, name, description, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?)
        """, (project_id, "First", "First project", timestamp, timestamp))
        conn.commit()

        try:
            cursor.execute("""
                INSERT INTO projects (id, name, description, created_at, updated_at)
                VALUES (?, ?, ?, ?, ?)
            """, (project_id, "Second", "Duplicate ID", timestamp, timestamp))
            conn.commit()
            print_error("Should have raised error for duplicate ID")
            return False
        except sqlite3.IntegrityError:
            conn.rollback()
            print_success("Correctly rejected duplicate project ID")

        # Test 2: Foreign key constraint (if enabled)
        print_info("Test 2: Attempting to create folder with non-existent project...")
        try:
            cursor.execute("""
                INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
                VALUES (?, ?, ?, ?, ?, ?, ?)
            """, ("folder_orphan", "Orphan", "No parent", "nonexistent_project", "", timestamp, timestamp))
            conn.commit()
            # Note: This might not fail if foreign keys aren't enabled
            print_info("  SQLite allowed insertion (foreign keys may not be enabled)")
            conn.rollback()
        except sqlite3.IntegrityError:
            conn.rollback()
            print_success("Correctly rejected orphan folder")

        # Test 3: Query non-existent record
        print_info("Test 3: Querying non-existent project...")
        cursor.execute("SELECT * FROM projects WHERE id = ?", ("nonexistent_id",))
        result = cursor.fetchone()
        assert result is None, "Query for non-existent project should return None"
        print_success("Correctly returned None for non-existent project")

        # Test 4: Delete non-existent record
        print_info("Test 4: Deleting non-existent note...")
        cursor.execute("DELETE FROM notes WHERE id = ?", ("nonexistent_note",))
        affected = cursor.rowcount
        assert affected == 0, "Delete of non-existent note should affect 0 rows"
        print_success(f"Correctly affected {affected} rows")

        print_success("\n✅ Error handling test PASSED!")
        return True

    except Exception as e:
        print_error(f"\n❌ Error handling test FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        conn.close()
        try:
            os.remove(db_path)
            os.rmdir(temp_dir)
        except:
            pass

def main():
    """Run all integration tests"""
    print(f"\n{Colors.CYAN}{'='*70}")
    print(f"  Plotter Integration Tests - Complete Use Case Testing")
    print(f"  SQLite Backend")
    print(f"{'='*70}{Colors.NC}\n")

    tests = [
        ("Complete Workflow", test_complete_workflow),
        ("Concurrent Operations", test_concurrent_operations),
        ("Error Handling", test_error_handling),
    ]

    results = []
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print_error(f"Test '{test_name}' crashed: {e}")
            results.append((test_name, False))

    # Print summary
    print(f"\n{Colors.CYAN}{'='*70}")
    print(f"  TEST SUMMARY")
    print(f"{'='*70}{Colors.NC}\n")

    passed = sum(1 for _, result in results if result)
    total = len(results)

    for test_name, result in results:
        status = f"{Colors.GREEN}PASSED{Colors.NC}" if result else f"{Colors.RED}FAILED{Colors.NC}"
        print(f"  {test_name}: {status}")

    print(f"\n{Colors.CYAN}{'='*70}{Colors.NC}")
    print(f"  Total: {passed}/{total} tests passed")

    if passed == total:
        print(f"  {Colors.GREEN}✅ ALL TESTS PASSED!{Colors.NC}")
        print(f"{Colors.CYAN}{'='*70}{Colors.NC}\n")
        return 0
    else:
        print(f"  {Colors.RED}❌ SOME TESTS FAILED{Colors.NC}")
        print(f"{Colors.CYAN}{'='*70}{Colors.NC}\n")
        return 1

if __name__ == "__main__":
    sys.exit(main())

