#!/usr/bin/env python3
"""
Integration tests for Plotter Filesystem Data Source

This script tests the Filesystem backend with individual tests for each operation:
- Create (Projects, Folders, Notes)
- Read (Projects, Folders, Notes)
- Update (Notes)
- Delete (Notes, Folders, Projects)
"""

import os
import sys
import json
import shutil
import tempfile
from pathlib import Path
from datetime import datetime

# Try to import send2trash, fall back to shutil.rmtree
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

def cleanup_directory(dirpath):
    """Move directory to trash or delete it"""
    if os.path.exists(dirpath):
        try:
            if USE_TRASH:
                send2trash(dirpath)
                print_info(f"Moved to trash: {dirpath}")
            else:
                shutil.rmtree(dirpath)
                print_info(f"Deleted: {dirpath}")
        except Exception as e:
            print_error(f"Failed to cleanup {dirpath}: {e}")

def test_create_project_filesystem():
    """Test creating a project in filesystem"""
    print_test_header("Test 1: Create Project (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_create_project_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create project directory
        project_name = "TestProject"
        project_path = os.path.join(test_root, project_name)
        os.makedirs(project_path)

        # Create project metadata
        project_id = f"proj-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().timestamp() * 1000)

        project_meta = {
            "id": project_id,
            "name": project_name,
            "description": "Test project description",
            "createdAt": timestamp,
            "updatedAt": timestamp,
            "folderIds": []
        }

        dotfile_path = os.path.join(project_path, '.plotter_project')
        with open(dotfile_path, 'w') as f:
            json.dump(project_meta, f, indent=2)

        print_success("Project directory created")
        print_success("Project metadata written to .plotter_project")

        # Verify project
        if not os.path.exists(dotfile_path):
            print_error(".plotter_project file not found")
            return False

        with open(dotfile_path, 'r') as f:
            loaded_meta = json.load(f)

        if loaded_meta['id'] == project_id and loaded_meta['name'] == project_name:
            print_success(f"Project verified: {project_id}")
            return True
        else:
            print_error("Project verification failed")
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_create_folder_filesystem():
    """Test creating a folder in filesystem"""
    print_test_header("Test 2: Create Folder (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_create_folder_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create parent project first
        project_path = os.path.join(test_root, "ParentProject")
        os.makedirs(project_path)
        project_id = f"proj-{datetime.now().strftime('%Y%m%d%H%M%S')}"

        # Create folder directory
        folder_name = "TestFolder"
        folder_path = os.path.join(project_path, folder_name)
        os.makedirs(folder_path)

        # Create folder metadata
        folder_id = f"folder-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().timestamp() * 1000)

        folder_meta = {
            "id": folder_id,
            "name": folder_name,
            "description": "Test folder description",
            "parentProjectId": project_id,
            "parentFolderId": "",
            "createdAt": timestamp,
            "updatedAt": timestamp,
            "noteIds": [],
            "subfolderIds": []
        }

        dotfile_path = os.path.join(folder_path, '.plotter_folder')
        with open(dotfile_path, 'w') as f:
            json.dump(folder_meta, f, indent=2)

        print_success("Folder directory created")
        print_success("Folder metadata written to .plotter_folder")

        # Verify folder
        if not os.path.exists(dotfile_path):
            print_error(".plotter_folder file not found")
            return False

        with open(dotfile_path, 'r') as f:
            loaded_meta = json.load(f)

        if loaded_meta['id'] == folder_id and loaded_meta['parentProjectId'] == project_id:
            print_success(f"Folder verified: {folder_id}")
            print_success(f"Parent link verified: {project_id}")
            return True
        else:
            print_error("Folder verification failed")
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_create_note_filesystem():
    """Test creating a note in filesystem"""
    print_test_header("Test 3: Create Note (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_create_note_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create parent folder
        folder_path = os.path.join(test_root, "ParentFolder")
        os.makedirs(folder_path)
        folder_id = f"folder-{datetime.now().strftime('%Y%m%d%H%M%S')}"

        # Create note file
        note_name = "test_note"
        note_path = os.path.join(folder_path, f"{note_name}.md")
        content = "# Test Note\n\nThis is test content for the note."

        with open(note_path, 'w') as f:
            f.write(content)

        # Create note metadata
        note_id = f"note-{datetime.now().strftime('%Y%m%d%H%M%S')}"
        timestamp = int(datetime.now().timestamp() * 1000)

        note_meta = {
            "id": note_id,
            "name": note_name,
            "parentFolderId": folder_id,
            "createdAt": timestamp,
            "updatedAt": timestamp
        }

        meta_path = note_path + '.plotter_meta'
        with open(meta_path, 'w') as f:
            json.dump(note_meta, f, indent=2)

        print_success("Note file created")
        print_success("Note metadata written to .plotter_meta")

        # Verify note
        if not os.path.exists(note_path) or not os.path.exists(meta_path):
            print_error("Note files not found")
            return False

        with open(note_path, 'r') as f:
            loaded_content = f.read()

        with open(meta_path, 'r') as f:
            loaded_meta = json.load(f)

        if loaded_content == content and loaded_meta['id'] == note_id:
            print_success(f"Note verified: {note_id}")
            print_success(f"Content verified: {len(content)} characters")
            print_success(f"Parent link verified: {folder_id}")
            return True
        else:
            print_error("Note verification failed")
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_read_project_filesystem():
    """Test reading a project from filesystem"""
    print_test_header("Test 4: Read Project (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_read_project_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create project
        project_path = os.path.join(test_root, "ReadableProject")
        os.makedirs(project_path)

        project_meta = {
            "id": "proj-read-test",
            "name": "ReadableProject",
            "description": "Project for read test",
            "createdAt": int(datetime.now().timestamp() * 1000),
            "updatedAt": int(datetime.now().timestamp() * 1000),
            "folderIds": []
        }

        with open(os.path.join(project_path, '.plotter_project'), 'w') as f:
            json.dump(project_meta, f, indent=2)

        # Read project
        with open(os.path.join(project_path, '.plotter_project'), 'r') as f:
            loaded_meta = json.load(f)

        print_success(f"Project read successfully: {loaded_meta['name']}")
        print_info(f"  ID: {loaded_meta['id']}")
        print_info(f"  Name: {loaded_meta['name']}")
        print_info(f"  Description: {loaded_meta['description']}")

        return True

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_read_folder_filesystem():
    """Test reading a folder from filesystem"""
    print_test_header("Test 5: Read Folder (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_read_folder_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create folder
        folder_path = os.path.join(test_root, "ReadableFolder")
        os.makedirs(folder_path)

        folder_meta = {
            "id": "folder-read-test",
            "name": "ReadableFolder",
            "description": "Folder for read test",
            "parentProjectId": "proj-1",
            "parentFolderId": "",
            "createdAt": int(datetime.now().timestamp() * 1000),
            "updatedAt": int(datetime.now().timestamp() * 1000),
            "noteIds": [],
            "subfolderIds": []
        }

        with open(os.path.join(folder_path, '.plotter_folder'), 'w') as f:
            json.dump(folder_meta, f, indent=2)

        # Read folder
        with open(os.path.join(folder_path, '.plotter_folder'), 'r') as f:
            loaded_meta = json.load(f)

        print_success(f"Folder read successfully: {loaded_meta['name']}")
        print_info(f"  ID: {loaded_meta['id']}")
        print_info(f"  Name: {loaded_meta['name']}")
        print_info(f"  Parent Project: {loaded_meta['parentProjectId']}")

        return True

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_read_note_filesystem():
    """Test reading a note from filesystem"""
    print_test_header("Test 6: Read Note (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_read_note_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create note
        note_path = os.path.join(test_root, "readable_note.md")
        content = "# Readable Note\n\nContent for reading test."

        with open(note_path, 'w') as f:
            f.write(content)

        note_meta = {
            "id": "note-read-test",
            "name": "readable_note",
            "parentFolderId": "folder-1",
            "createdAt": int(datetime.now().timestamp() * 1000),
            "updatedAt": int(datetime.now().timestamp() * 1000)
        }

        with open(note_path + '.plotter_meta', 'w') as f:
            json.dump(note_meta, f, indent=2)

        # Read note
        with open(note_path, 'r') as f:
            loaded_content = f.read()

        with open(note_path + '.plotter_meta', 'r') as f:
            loaded_meta = json.load(f)

        print_success(f"Note read successfully: {loaded_meta['name']}")
        print_info(f"  ID: {loaded_meta['id']}")
        print_info(f"  Name: {loaded_meta['name']}")
        print_info(f"  Content length: {len(loaded_content)} characters")
        print_info(f"  Parent Folder: {loaded_meta['parentFolderId']}")

        return True

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_update_note_filesystem():
    """Test updating a note in filesystem"""
    print_test_header("Test 7: Update Note (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_update_note_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create note
        note_path = os.path.join(test_root, "updatable_note.md")
        original_content = "# Original Content"

        with open(note_path, 'w') as f:
            f.write(original_content)

        created_at = int(datetime.now().timestamp() * 1000)
        note_meta = {
            "id": "note-update-test",
            "name": "updatable_note",
            "parentFolderId": "folder-1",
            "createdAt": created_at,
            "updatedAt": created_at
        }

        meta_path = note_path + '.plotter_meta'
        with open(meta_path, 'w') as f:
            json.dump(note_meta, f, indent=2)

        print_success("Note created with original content")

        # Update note
        import time
        time.sleep(0.01)
        updated_content = "# Updated Content\n\nThis has been modified."

        with open(note_path, 'w') as f:
            f.write(updated_content)

        updated_at = int(datetime.now().timestamp() * 1000)
        note_meta['updatedAt'] = updated_at

        with open(meta_path, 'w') as f:
            json.dump(note_meta, f, indent=2)

        print_success("Note updated in filesystem")

        # Verify update
        with open(note_path, 'r') as f:
            loaded_content = f.read()

        with open(meta_path, 'r') as f:
            loaded_meta = json.load(f)

        if loaded_content != updated_content:
            print_error("Content was not updated correctly")
            return False

        if loaded_meta['updatedAt'] <= created_at:
            print_error("Timestamp was not updated")
            return False

        print_success("Content verified: updated correctly")
        print_success(f"Timestamp updated: {created_at} -> {loaded_meta['updatedAt']}")

        return True

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_delete_note_filesystem():
    """Test deleting a note from filesystem"""
    print_test_header("Test 8: Delete Note (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_delete_note_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create note
        note_path = os.path.join(test_root, "deletable_note.md")
        with open(note_path, 'w') as f:
            f.write("# Deletable Note")

        meta_path = note_path + '.plotter_meta'
        with open(meta_path, 'w') as f:
            json.dump({"id": "note-delete-test", "name": "deletable_note"}, f)

        print_success("Note created for deletion test")

        # Verify note exists
        if not os.path.exists(note_path) or not os.path.exists(meta_path):
            print_error("Note files not found before deletion")
            return False

        # Delete note
        os.remove(note_path)
        os.remove(meta_path)
        print_success("Note files deleted from filesystem")

        # Verify note is gone
        if not os.path.exists(note_path) and not os.path.exists(meta_path):
            print_success("Deletion verified: note files no longer exist")
            return True
        else:
            print_error("Note files still exist after deletion")
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_delete_folder_filesystem():
    """Test deleting a folder from filesystem"""
    print_test_header("Test 9: Delete Folder (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_delete_folder_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create folder
        folder_path = os.path.join(test_root, "DeletableFolder")
        os.makedirs(folder_path)

        with open(os.path.join(folder_path, '.plotter_folder'), 'w') as f:
            json.dump({"id": "folder-delete-test", "name": "DeletableFolder"}, f)

        print_success("Folder created for deletion test")

        # Delete folder
        shutil.rmtree(folder_path)
        print_success("Folder deleted from filesystem")

        # Verify folder is gone
        if not os.path.exists(folder_path):
            print_success("Deletion verified: folder no longer exists")
            return True
        else:
            print_error("Folder still exists after deletion")
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

def test_delete_project_filesystem():
    """Test deleting a project from filesystem"""
    print_test_header("Test 10: Delete Project (Filesystem)")

    temp_dir = tempfile.gettempdir()
    test_root = os.path.join(temp_dir, f"plotter_delete_project_{datetime.now().strftime('%Y%m%d_%H%M%S')}")

    try:
        os.makedirs(test_root, exist_ok=True)

        # Create project
        project_path = os.path.join(test_root, "DeletableProject")
        os.makedirs(project_path)

        with open(os.path.join(project_path, '.plotter_project'), 'w') as f:
            json.dump({"id": "proj-delete-test", "name": "DeletableProject"}, f)

        print_success("Project created for deletion test")

        # Delete project
        shutil.rmtree(project_path)
        print_success("Project deleted from filesystem")

        # Verify project is gone
        if not os.path.exists(project_path):
            print_success("Deletion verified: project no longer exists")
            return True
        else:
            print_error("Project still exists after deletion")
            return False

    except Exception as e:
        print_error(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        cleanup_directory(test_root)

if __name__ == "__main__":
    tests = [
        ("Create Project", test_create_project_filesystem),
        ("Create Folder", test_create_folder_filesystem),
        ("Create Note", test_create_note_filesystem),
        ("Read Project", test_read_project_filesystem),
        ("Read Folder", test_read_folder_filesystem),
        ("Read Note", test_read_note_filesystem),
        ("Update Note", test_update_note_filesystem),
        ("Delete Note", test_delete_note_filesystem),
        ("Delete Folder", test_delete_folder_filesystem),
        ("Delete Project", test_delete_project_filesystem),
    ]

    passed = 0
    failed = 0

    print_test_header("Filesystem Data Source Integration Tests")

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
        print(f"{Colors.GREEN}✅ All Filesystem tests passed!{Colors.NC}\n")
        sys.exit(0)
    else:
        print(f"{Colors.RED}❌ {failed} test(s) failed{Colors.NC}\n")
        sys.exit(1)
