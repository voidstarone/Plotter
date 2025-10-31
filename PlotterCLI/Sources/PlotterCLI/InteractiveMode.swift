import Foundation
import PlotterObjCBridge

class InteractiveMode {
    private let plotter: PLTPlotter
    private let databasePath: String
    private var running = true
    
    init(databasePath: String) throws {
        self.databasePath = databasePath
        guard let plotter = PLTPlotter(databasePath: databasePath) else {
            throw InteractiveModeError.initializationFailed
        }
        self.plotter = plotter
    }
    
    func run() {
        clearScreen()
        showWelcome()
        
        while running {
            showMainMenu()
            if let choice = readChoice() {
                handleMainChoice(choice)
            }
        }
        
        print("\n👋 Goodbye!\n")
    }
    
    // MARK: - Screen Management
    
    private func clearScreen() {
        print("\u{001B}[2J\u{001B}[H", terminator: "")
        fflush(stdout)
    }
    
    private func showWelcome() {
        print("""
        ╔════════════════════════════════════════════════════════════╗
        ║                                                            ║
        ║                    📝 PLOTTER TUI                          ║
        ║         Interactive Note Management System                 ║
        ║                                                            ║
        ╚════════════════════════════════════════════════════════════╝
        
        Database: \(databasePath)
        
        """)
    }
    
    // MARK: - Main Menu
    
    private func showMainMenu() {
        print("""
        
        ┌─────────────────────────────────────────────────────────────┐
        │                        MAIN MENU                            │
        ├─────────────────────────────────────────────────────────────┤
        │  1. 📋 Browse projects                                      │
        │  2. ➕ Create new project                                   │
        │  3. 🔍 Explore project (folders & notes)                    │
        │  4. 📊 Show statistics                                      │
        │  0. 🚪 Exit                                                 │
        └─────────────────────────────────────────────────────────────┘
        
        """)
        print("Enter your choice: ", terminator: "")
        fflush(stdout)
    }
    
    private func handleMainChoice(_ choice: String) {
        print()
        
        switch choice {
        case "1":
            listProjects()
        case "2":
            createProject()
        case "3":
            exploreProject()
        case "4":
            showStatistics()
        case "0":
            running = false
        default:
            print("❌ Invalid choice. Please try again.")
        }
        
        if running {
            print("\nPress Enter to continue...")
            _ = readLine()
            clearScreen()
            showWelcome()
        }
    }
    
    // MARK: - Input Helpers
    
    private func readChoice() -> String? {
        return readLine()?.trimmingCharacters(in: .whitespaces)
    }
    
    private func readInput(prompt: String) -> String {
        print(prompt, terminator: "")
        fflush(stdout)
        return readLine()?.trimmingCharacters(in: .whitespaces) ?? ""
    }
    
    private func readMultilineInput(prompt: String) -> String {
        print(prompt)
        print("(Enter a line with just '.' to finish)")
        var lines: [String] = []
        while true {
            if let line = readLine() {
                if line == "." {
                    break
                }
                lines.append(line)
            }
        }
        return lines.joined(separator: "\n")
    }
    
    // MARK: - Project Operations
    
    private func listProjects() {
        print("═══════════════════════════════════════════════════════════")
        print("                    📋 ALL PROJECTS")
        print("═══════════════════════════════════════════════════════════\n")
        
        do {
            let projects = try plotter.listProjects()
            
            if projects.isEmpty {
                print("  No projects found. Create one with option 2!\n")
                return
            }
            
            for (index, project) in projects.enumerated() {
                print("  \(index + 1). \(project.name)")
                print("     ID: \(project.identifier)")
                if !project.projectDescription.isEmpty {
                    print("     📝 \(project.projectDescription)")
                }
                print()
            }
            
            print("Total: \(projects.count) project(s)")
            
        } catch {
            print("❌ Error listing projects: \(error.localizedDescription)")
        }
    }
    
    private func createProject() {
        print("═══════════════════════════════════════════════════════════")
        print("                   ➕ CREATE NEW PROJECT")
        print("═══════════════════════════════════════════════════════════\n")
        
        let name = readInput(prompt: "Project name: ")
        guard !name.isEmpty else {
            print("❌ Project name cannot be empty.")
            return
        }
        
        let description = readInput(prompt: "Description (optional): ")
        
        do {
            let projectId = try plotter.createProject(withName: name, description: description)
            print("\n✅ Project '\(name)' created successfully!")
            print("   ID: \(projectId)")
        } catch {
            print("\n❌ Error creating project: \(error.localizedDescription)")
        }
    }
    
    // MARK: - Project Exploration (Hierarchical Navigation)
    
    private func exploreProject() {
        do {
            let projects = try plotter.listProjects()
            
            if projects.isEmpty {
                print("  No projects available. Create one first!\n")
                return
            }
            
            print("═══════════════════════════════════════════════════════════")
            print("                   🔍 SELECT PROJECT")
            print("═══════════════════════════════════════════════════════════\n")
            
            for (index, project) in projects.enumerated() {
                print("  \(index + 1). \(project.name)")
                if !project.projectDescription.isEmpty {
                    print("      \(project.projectDescription)")
                }
            }
            print()
            
            let choice = readInput(prompt: "Select project number (0 to cancel): ")
            guard let index = Int(choice), index > 0, index <= projects.count else {
                if choice != "0" {
                    print("❌ Invalid selection.")
                }
                return
            }
            
            let project = projects[index - 1]
            showProjectMenu(project: project)
            
        } catch {
            print("❌ Error: \(error.localizedDescription)")
        }
    }
    
    private func showProjectMenu(project: PLTProject) {
        var exploring = true
        
        while exploring {
            clearScreen()
            print("""
            ╔════════════════════════════════════════════════════════════╗
            ║  PROJECT: \(project.name.padding(toLength: 48, withPad: " ", startingAt: 0))║
            ╚════════════════════════════════════════════════════════════╝
            
            ID: \(project.identifier)
            
            ┌─────────────────────────────────────────────────────────────┐
            │                     PROJECT MENU                            │
            ├─────────────────────────────────────────────────────────────┤
            │  1. 📁 List folders                                         │
            │  2. ➕ Create new folder                                    │
            │  3. 🔍 Browse folder (view notes)                           │
            │  0. ⬅️  Back to main menu                                   │
            └─────────────────────────────────────────────────────────────┘
            
            """)
            
            print("Enter your choice: ", terminator: "")
            fflush(stdout)
            
            if let choice = readChoice() {
                print()
                switch choice {
                case "1":
                    listFolders(projectId: project.identifier)
                    print("\nPress Enter to continue...")
                    _ = readLine()
                case "2":
                    createFolder(projectId: project.identifier)
                    print("\nPress Enter to continue...")
                    _ = readLine()
                case "3":
                    browseFolder(projectId: project.identifier)
                case "0":
                    exploring = false
                default:
                    print("❌ Invalid choice.")
                    print("\nPress Enter to continue...")
                    _ = readLine()
                }
            }
        }
    }
    
    // MARK: - Folder Operations
    
    private func listFolders(projectId: String) {
        print("═══════════════════════════════════════════════════════════")
        print("                    📁 FOLDERS")
        print("═══════════════════════════════════════════════════════════\n")
        
        do {
            let folders = try plotter.listFolders(inProject: projectId)
            
            if folders.isEmpty {
                print("  No folders in this project. Create one with option 2!\n")
                return
            }
            
            for (index, folder) in folders.enumerated() {
                print("  \(index + 1). 📁 \(folder.name)")
                print("      ID: \(folder.identifier)")
                if !folder.folderDescription.isEmpty {
                    print("      📝 \(folder.folderDescription)")
                }
                print()
            }
            
            print("Total: \(folders.count) folder(s)")
            
        } catch {
            print("❌ Error listing folders: \(error.localizedDescription)")
        }
    }
    
    private func createFolder(projectId: String) {
        print("═══════════════════════════════════════════════════════════")
        print("                  ➕ CREATE NEW FOLDER")
        print("═══════════════════════════════════════════════════════════\n")
        
        let name = readInput(prompt: "Folder name: ")
        guard !name.isEmpty else {
            print("❌ Folder name cannot be empty.")
            return
        }
        
        let description = readInput(prompt: "Description (optional): ")
        
        do {
            let folderId = try plotter.createFolder(
                withName: name,
                description: description,
                parentProjectId: projectId,
                parentFolderId: nil
            )
            print("\n✅ Folder '\(name)' created successfully!")
            print("   ID: \(folderId)")
        } catch {
            print("\n❌ Error creating folder: \(error.localizedDescription)")
        }
    }
    
    private func browseFolder(projectId: String) {
        do {
            let folders = try plotter.listFolders(inProject: projectId)
            
            if folders.isEmpty {
                print("  No folders available. Create one first!\n")
                print("\nPress Enter to continue...")
                _ = readLine()
                return
            }
            
            print("═══════════════════════════════════════════════════════════")
            print("                   🔍 SELECT FOLDER")
            print("═══════════════════════════════════════════════════════════\n")
            
            for (index, folder) in folders.enumerated() {
                print("  \(index + 1). 📁 \(folder.name)")
            }
            print()
            
            let choice = readInput(prompt: "Select folder number (0 to cancel): ")
            guard let index = Int(choice), index > 0, index <= folders.count else {
                if choice != "0" {
                    print("❌ Invalid selection.")
                    print("\nPress Enter to continue...")
                    _ = readLine()
                }
                return
            }
            
            let folder = folders[index - 1]
            showFolderMenu(folder: folder)
            
        } catch {
            print("❌ Error: \(error.localizedDescription)")
            print("\nPress Enter to continue...")
            _ = readLine()
        }
    }
    
    private func showFolderMenu(folder: PLTFolder) {
        var exploring = true
        
        while exploring {
            clearScreen()
            print("""
            ╔════════════════════════════════════════════════════════════╗
            ║  FOLDER: \(folder.name.padding(toLength: 49, withPad: " ", startingAt: 0))║
            ╚════════════════════════════════════════════════════════════╝
            
            ID: \(folder.identifier)
            
            ┌─────────────────────────────────────────────────────────────┐
            │                      FOLDER MENU                            │
            ├─────────────────────────────────────────────────────────────┤
            │  1. 📄 List notes                                           │
            │  2. ➕ Create new note                                      │
            │  3. 👁️  View note content                                   │
            │  4. ✏️  Edit note                                           │
            │  0. ⬅️  Back to project                                     │
            └─────────────────────────────────────────────────────────────┘
            
            """)
            
            print("Enter your choice: ", terminator: "")
            fflush(stdout)
            
            if let choice = readChoice() {
                print()
                switch choice {
                case "1":
                    listNotes(folderId: folder.identifier)
                    print("\nPress Enter to continue...")
                    _ = readLine()
                case "2":
                    createNote(folderId: folder.identifier)
                    print("\nPress Enter to continue...")
                    _ = readLine()
                case "3":
                    viewNote(folderId: folder.identifier)
                    print("\nPress Enter to continue...")
                    _ = readLine()
                case "4":
                    editNote(folderId: folder.identifier)
                    print("\nPress Enter to continue...")
                    _ = readLine()
                case "0":
                    exploring = false
                default:
                    print("❌ Invalid choice.")
                    print("\nPress Enter to continue...")
                    _ = readLine()
                }
            }
        }
    }
    
    // MARK: - Note Operations
    
    private func listNotes(folderId: String) {
        print("═══════════════════════════════════════════════════════════")
        print("                     📄 NOTES")
        print("═══════════════════════════════════════════════════════════\n")
        
        do {
            let notes = try plotter.listNotes(inFolder: folderId)
            
            if notes.isEmpty {
                print("  No notes in this folder. Create one with option 2!\n")
                return
            }
            
            for (index, note) in notes.enumerated() {
                print("  \(index + 1). 📄 \(note.name)")
                print("      ID: \(note.identifier)")
                print("      Path: \(note.path)")
                print()
            }
            
            print("Total: \(notes.count) note(s)")
            
        } catch {
            print("❌ Error listing notes: \(error.localizedDescription)")
        }
    }
    
    private func createNote(folderId: String) {
        print("═══════════════════════════════════════════════════════════")
        print("                   ➕ CREATE NEW NOTE")
        print("═══════════════════════════════════════════════════════════\n")
        
        let name = readInput(prompt: "Note name: ")
        guard !name.isEmpty else {
            print("❌ Note name cannot be empty.")
            return
        }
        
        let content = readMultilineInput(prompt: "\nNote content:")
        
        do {
            let noteId = try plotter.createNote(
                withName: name,
                parentFolderId: folderId,
                initialContent: content
            )
            print("\n✅ Note '\(name)' created successfully!")
            print("   ID: \(noteId)")
        } catch {
            print("\n❌ Error creating note: \(error.localizedDescription)")
        }
    }
    
    private func viewNote(folderId: String) {
        do {
            let notes = try plotter.listNotes(inFolder: folderId)
            
            if notes.isEmpty {
                print("  No notes available. Create one first!\n")
                return
            }
            
            print("═══════════════════════════════════════════════════════════")
            print("                    👁️  SELECT NOTE")
            print("═══════════════════════════════════════════════════════════\n")
            
            for (index, note) in notes.enumerated() {
                print("  \(index + 1). 📄 \(note.name)")
            }
            print()
            
            let choice = readInput(prompt: "Select note number (0 to cancel): ")
            guard let index = Int(choice), index > 0, index <= notes.count else {
                if choice != "0" {
                    print("❌ Invalid selection.")
                }
                return
            }
            
            let note = notes[index - 1]
            let content = try plotter.getNoteContent(note.identifier)
            
            print("\n┌─────────────────────────────────────────────────────────┐")
            print("│ \(note.name.padding(toLength: 57, withPad: " ", startingAt: 0))│")
            print("├─────────────────────────────────────────────────────────┤")
            print("│ ID: \(note.identifier.padding(toLength: 53, withPad: " ", startingAt: 0))│")
            print("└─────────────────────────────────────────────────────────┘\n")
            print(content)
            print()
            
        } catch {
            print("❌ Error: \(error.localizedDescription)")
        }
    }
    
    private func editNote(folderId: String) {
        do {
            let notes = try plotter.listNotes(inFolder: folderId)
            
            if notes.isEmpty {
                print("  No notes available. Create one first!\n")
                return
            }
            
            print("═══════════════════════════════════════════════════════════")
            print("                    ✏️  EDIT NOTE")
            print("═══════════════════════════════════════════════════════════\n")
            
            for (index, note) in notes.enumerated() {
                print("  \(index + 1). 📄 \(note.name)")
            }
            print()
            
            let choice = readInput(prompt: "Select note number (0 to cancel): ")
            guard let index = Int(choice), index > 0, index <= notes.count else {
                if choice != "0" {
                    print("❌ Invalid selection.")
                }
                return
            }
            
            let note = notes[index - 1]
            let currentContent = try plotter.getNoteContent(note.identifier)
            
            print("\n📄 Current content:")
            print("─────────────────────────────────────────────────────────")
            print(currentContent)
            print("─────────────────────────────────────────────────────────\n")
            
            let newContent = readMultilineInput(prompt: "Enter new content:")
            
            try plotter.updateNoteContent(note.identifier, content: newContent)
            print("\n✅ Note '\(note.name)' updated successfully!")
            
        } catch {
            print("❌ Error: \(error.localizedDescription)")
        }
    }
    
    // MARK: - Statistics
    
    private func showStatistics() {
        print("═══════════════════════════════════════════════════════════")
        print("                      📊 STATISTICS")
        print("═══════════════════════════════════════════════════════════\n")
        
        do {
            let projects = try plotter.listProjects()
            
            var totalFolders = 0
            var totalNotes = 0
            
            for project in projects {
                if let folders = try? plotter.listFolders(inProject: project.identifier) {
                    totalFolders += folders.count
                    for folder in folders {
                        if let notes = try? plotter.listNotes(inFolder: folder.identifier) {
                            totalNotes += notes.count
                        }
                    }
                }
            }
            
            print("┌─────────────────────────────────────────────────────────┐")
            print("│ Database Statistics")
            print("├─────────────────────────────────────────────────────────┤")
            print("│ Database: \(databasePath.padding(toLength: 46, withPad: " ", startingAt: 0))│")
            print("│                                                         │")
            print("│ 📋 Total Projects: \(String(projects.count).padding(toLength: 38, withPad: " ", startingAt: 0))│")
            print("│ 📁 Total Folders:  \(String(totalFolders).padding(toLength: 38, withPad: " ", startingAt: 0))│")
            print("│ 📄 Total Notes:    \(String(totalNotes).padding(toLength: 38, withPad: " ", startingAt: 0))│")
            print("└─────────────────────────────────────────────────────────┘")
            
        } catch {
            print("❌ Error gathering statistics: \(error.localizedDescription)")
        }
    }
}

enum InteractiveModeError: LocalizedError {
    case initializationFailed
    
    var errorDescription: String? {
        switch self {
        case .initializationFailed:
            return "Failed to initialize Plotter"
        }
    }
}
