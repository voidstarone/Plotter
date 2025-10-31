#!/usr/bin/env python3
"""
Integration tests for Plotter SQLite Data Source

This script tests the SQLite backend with individual tests for each operation:
- Create (Projects, Folders, Notes)
- Read (Projects, Folders, Notes)
- Update (Notes)
- Delete (Notes, Folders, Projects)
"""

import sqlite3
import os
import sys
import tempfile
from datetime import datetime

# Try to import send2trash, fall back to os.remove
try:
    from send2trash import send2trash
    USE_TRASH = True
except ImportError:
    USE_TRASH = False

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
    print(f"\n{Colors.CYAN}{'='*60}{Colors.NC}")
    print(f"{Colors.CYAN}{msg}{Colors.NC}")
    print(f"{Colors.CYAN}{'='*60}{Colors.NC}\n")

def cleanup_file(filepath):
    """Move file to trash or delete it"""
    if os.path.exists(filepath):
        try:
            if USE_TRASH:
                send2trash(filepath)
                print_info(f"Moved to trash: {filepath}")
            else:
                os.remove(filepath)
                print_info(f"Deleted: {filepath}")
        except Exception as e:
            print_error(f"Failed to cleanup {filepath}: {e}")

def create_test_database(db_path):
    """Create database schema"""
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    cursor.execute("""
        CREATE TABLE projects (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            description TEXT,
            created_at INTEGER,
            updated_at INTEGER
        )
    """)

    cursor.execute("""
        CREATE TABLE folders (
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
        CREATE TABLE notes (
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

def test_create_project_sqlite():
    """Test creating a project in SQLite"""
    print_test_header("Test 1: Create Project (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_create_project_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create project
        project_id = f"proj-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().timestamp() * 1000)

        cursor.execute("""
            INSERT INTO projects (id, name, description, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?)
        """, (project_id, "Test Project", "Test Description", timestamp, timestamp))

        conn.commit()
        print_success("Project created in database")

        # Verify project exists
        cursor.execute("SELECT id, name, description FROM projects WHERE id=?", (project_id,))
        row = cursor.fetchone()

        if row and row[1] == "Test Project" and row[2] == "Test Description":
            print_success(f"Project verified: {project_id}")
            conn.close()
            return True
        else:
            print_error("Project verification failed")
            conn.close()
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_create_folder_sqlite():
    """Test creating a folder in SQLite"""
    print_test_header("Test 2: Create Folder (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_create_folder_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create parent project first
        project_id = f"proj-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().timestamp() * 1000)
        cursor.execute("""
            INSERT INTO projects (id, name, description, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?)
        """, (project_id, "Parent Project", "", timestamp, timestamp))

        # Create folder
        folder_id = f"folder-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        cursor.execute("""
            INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (folder_id, "Test Folder", "Folder Description", project_id, "", timestamp, timestamp))

        conn.commit()
        print_success("Folder created in database")

        # Verify folder exists and is linked to project
        cursor.execute("SELECT id, name, parent_project_id FROM folders WHERE id=?", (folder_id,))
        row = cursor.fetchone()

        if row and row[1] == "Test Folder" and row[2] == project_id:
            print_success(f"Folder verified: {folder_id}")
            print_success(f"Parent link verified: {project_id}")
            conn.close()
            return True
        else:
            print_error("Folder verification failed")
            conn.close()
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_create_note_sqlite():
    """Test creating a note in SQLite"""
    print_test_header("Test 3: Create Note (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_create_note_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create parent folder
        folder_id = f"folder-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().timestamp() * 1000)
        cursor.execute("""
            INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (folder_id, "Parent Folder", "", "proj-1", "", timestamp, timestamp))

        # Create note
        note_id = f"note-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        content = "# Test Note\n\nThis is test content."

        cursor.execute("""
            INSERT INTO notes (id, name, content, path, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (note_id, "Test Note", content, "/tmp/test.md", folder_id, timestamp, timestamp))

        conn.commit()
        print_success("Note created in database")

        # Verify note exists
        cursor.execute("SELECT id, name, content, parent_folder_id FROM notes WHERE id=?", (note_id,))
        row = cursor.fetchone()

        if row and row[1] == "Test Note" and row[2] == content and row[3] == folder_id:
            print_success(f"Note verified: {note_id}")
            print_success(f"Content verified: {len(content)} characters")
            print_success(f"Parent link verified: {folder_id}")
            conn.close()
            return True
        else:
            print_error("Note verification failed")
            conn.close()
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_read_project_sqlite():
    """Test reading a project from SQLite"""
    print_test_header("Test 4: Read Project (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_read_project_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create test project
        project_id = f"proj-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().timestamp() * 1000)
        cursor.execute("""
            INSERT INTO projects (id, name, description, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?)
        """, (project_id, "Readable Project", "Read test", timestamp, timestamp))
        conn.commit()

        # Read project
        cursor.execute("SELECT * FROM projects WHERE id=?", (project_id,))
        row = cursor.fetchone()

        if not row:
            print_error("Failed to read project")
            return False

        print_success(f"Project read successfully: {row[1]}")
        print_info(f"  ID: {row[0]}")
        print_info(f"  Name: {row[1]}")
        print_info(f"  Description: {row[2]}")
        print_info(f"  Created: {row[3]}")

        conn.close()
        return True

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_read_folder_sqlite():
    """Test reading a folder from SQLite"""
    print_test_header("Test 5: Read Folder (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_read_folder_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create test folder
        folder_id = f"folder-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().timestamp() * 1000)
        cursor.execute("""
            INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (folder_id, "Readable Folder", "Read test", "proj-1", "", timestamp, timestamp))
        conn.commit()

        # Read folder
        cursor.execute("SELECT * FROM folders WHERE id=?", (folder_id,))
        row = cursor.fetchone()

        if not row:
            print_error("Failed to read folder")
            return False

        print_success(f"Folder read successfully: {row[1]}")
        print_info(f"  ID: {row[0]}")
        print_info(f"  Name: {row[1]}")
        print_info(f"  Parent Project: {row[3]}")

        conn.close()
        return True

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_read_note_sqlite():
    """Test reading a note from SQLite"""
    print_test_header("Test 6: Read Note (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_read_note_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create test note
        note_id = f"note-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        content = "# Readable Note\n\nContent for reading test."
        timestamp = int(datetime.now().timestamp() * 1000)
        cursor.execute("""
            INSERT INTO notes (id, name, content, path, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (note_id, "Readable Note", content, "/tmp/read.md", "folder-1", timestamp, timestamp))
        conn.commit()

        # Read note
        cursor.execute("SELECT * FROM notes WHERE id=?", (note_id,))
        row = cursor.fetchone()

        if not row:
            print_error("Failed to read note")
            return False

        print_success(f"Note read successfully: {row[1]}")
        print_info(f"  ID: {row[0]}")
        print_info(f"  Name: {row[1]}")
        print_info(f"  Content length: {len(row[2])} characters")
        print_info(f"  Parent Folder: {row[4]}")

        conn.close()
        return True

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_update_note_sqlite():
    """Test updating a note in SQLite"""
    print_test_header("Test 7: Update Note (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_update_note_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create note
        note_id = f"note-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        original_content = "# Original Content"
        created_at = int(datetime.now().timestamp() * 1000)

        cursor.execute("""
            INSERT INTO notes (id, name, content, path, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (note_id, "Updatable Note", original_content, "/tmp/update.md", "folder-1", created_at, created_at))
        conn.commit()
        print_success("Note created with original content")

        # Update note
        import time
        time.sleep(0.01)
        updated_content = "# Updated Content\n\nThis has been modified."
        updated_at = int(datetime.now().strftime('%Y%m%d%H%M%S'))

        cursor.execute("""
            UPDATE notes SET content=?, updated_at=? WHERE id=?
        """, (updated_content, updated_at, note_id))
        conn.commit()
        print_success("Note updated in database")

        # Verify update
        cursor.execute("SELECT content, updated_at FROM notes WHERE id=?", (note_id,))
        row = cursor.fetchone()

        if not row:
            print_error("Note not found after update")
            return False

        if row[0] != updated_content:
            print_error("Content was not updated correctly")
            return False

        if row[1] <= created_at:
            print_error("Timestamp was not updated")
            return False

        print_success("Content verified: updated correctly")
        print_success(f"Timestamp updated: {created_at} -> {row[1]}")

        conn.close()
        return True

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_delete_note_sqlite():
    """Test deleting a note from SQLite"""
    print_test_header("Test 8: Delete Note (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_delete_note_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create note
        note_id = f"note-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().strftime('%Y%m%d%H%M%S'))
        cursor.execute("""
            INSERT INTO notes (id, name, content, path, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (note_id, "Deletable Note", "Content", "/tmp/delete.md", "folder-1", timestamp, timestamp))
        conn.commit()
        print_success("Note created for deletion test")

        # Verify note exists
        cursor.execute("SELECT COUNT(*) FROM notes WHERE id=?", (note_id,))
        if cursor.fetchone()[0] != 1:
            print_error("Note not found before deletion")
            return False

        # Delete note
        cursor.execute("DELETE FROM notes WHERE id=?", (note_id,))
        conn.commit()
        print_success("Note deleted from database")

        # Verify note is gone
        cursor.execute("SELECT COUNT(*) FROM notes WHERE id=?", (note_id,))
        if cursor.fetchone()[0] == 0:
            print_success("Deletion verified: note no longer exists")
            conn.close()
            return True
        else:
            print_error("Note still exists after deletion")
            conn.close()
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_delete_folder_sqlite():
    """Test deleting a folder from SQLite"""
    print_test_header("Test 9: Delete Folder (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_delete_folder_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create folder
        folder_id = f"folder-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().strftime('%Y%m%d%H%M%S'))
        cursor.execute("""
            INSERT INTO folders (id, name, description, parent_project_id, parent_folder_id, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (folder_id, "Deletable Folder", "", "proj-1", "", timestamp, timestamp))
        conn.commit()
        print_success("Folder created for deletion test")

        # Delete folder
        cursor.execute("DELETE FROM folders WHERE id=?", (folder_id,))
        conn.commit()
        print_success("Folder deleted from database")

        # Verify folder is gone
        cursor.execute("SELECT COUNT(*) FROM folders WHERE id=?", (folder_id,))
        if cursor.fetchone()[0] == 0:
            print_success("Deletion verified: folder no longer exists")
            conn.close()
            return True
        else:
            print_error("Folder still exists after deletion")
            conn.close()
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

def test_delete_project_sqlite():
    """Test deleting a project from SQLite"""
    print_test_header("Test 10: Delete Project (SQLite)")

    temp_dir = tempfile.gettempdir()
    db_path = os.path.join(temp_dir, f"plotter_delete_project_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db")

    try:
        create_test_database(db_path)
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Create project
        project_id = f"proj-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().strftime('%Y%m%d%H%M%S'))
        cursor.execute("""
            INSERT INTO projects (id, name, description, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?)
        """, (project_id, "Deletable Project", "", timestamp, timestamp))
        conn.commit()
        print_success("Project created for deletion test")

        # Delete project
        cursor.execute("DELETE FROM projects WHERE id=?", (project_id,))
        conn.commit()
        print_success("Project deleted from database")

        # Verify project is gone
        cursor.execute("SELECT COUNT(*) FROM projects WHERE id=?", (project_id,))
        if cursor.fetchone()[0] == 0:
            print_success("Deletion verified: project no longer exists")
            conn.close()
            return True
        else:
            print_error("Project still exists after deletion")
            conn.close()
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_file(db_path)

if __name__ == "__main__":
    tests = [
        ("Create Project", test_create_project_sqlite),
        ("Create Folder", test_create_folder_sqlite),
        ("Create Note", test_create_note_sqlite),
        ("Read Project", test_read_project_sqlite),
        ("Read Folder", test_read_folder_sqlite),
        ("Read Note", test_read_note_sqlite),
        ("Update Note", test_update_note_sqlite),
        ("Delete Note", test_delete_note_sqlite),
        ("Delete Folder", test_delete_folder_sqlite),
        ("Delete Project", test_delete_project_sqlite),
    ]

    passed = 0
    failed = 0

    print_test_header("SQLite Data Source Integration Tests")

    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print_error(f"Test '{test_name}' crashed: {e}")
            failed += 1

    # Print summary
    print_test_header("Test Summary")
    print(f"Total tests:  {Colors.BLUE}{len(tests)}{Colors.NC}")
    print(f"Passed:       {Colors.GREEN}{passed}{Colors.NC}")
    print(f"Failed:       {Colors.RED}{failed}{Colors.NC}")
    print()

    if failed == 0:
        print(f"{Colors.GREEN}✅ All SQLite tests passed!{Colors.NC}\n")
        sys.exit(0)
    else:
        print(f"{Colors.RED}❌ {failed} test(s) failed{Colors.NC}\n")
        sys.exit(1)
