import Foundation
import ArgumentParser
import PlotterObjCBridge

@main
struct PlotterCLI: ParsableCommand {
    static let configuration = CommandConfiguration(
        commandName: "plotter",
        abstract: "A command-line tool for managing projects, folders, and notes",
        subcommands: [Project.self, Folder.self, Note.self, Interactive.self]
    )
    
    @Option(name: .shortAndLong, help: "Path to the database file")
    var database: String
    
    @Flag(name: .shortAndLong, help: "Enter interactive mode")
    var interactive: Bool = false
    
    mutating func run() throws {
        if interactive {
            let interactiveMode = try InteractiveMode(databasePath: database)
            interactiveMode.run()
        } else {
            throw CleanExit.helpRequest(self)
        }
    }
}

extension PlotterCLI {
    struct Project: ParsableCommand {
        static let configuration = CommandConfiguration(
            commandName: "project",
            abstract: "Manage projects",
            subcommands: [Create.self, List.self]
        )
        
        @OptionGroup var options: PlotterCLI
    }
}

extension PlotterCLI.Project {
    struct Create: ParsableCommand {
        static let configuration = CommandConfiguration(
            abstract: "Create a new project"
        )
        
        @OptionGroup var options: PlotterCLI
        
        @Option(name: .shortAndLong, help: "The project name")
        var name: String
        
        @Option(name: .shortAndLong, help: "The project description")
        var description: String = ""
        
        func run() throws {
            guard let plotter = PLTPlotter(databasePath: options.database) else {
                throw CLIError.initializationFailed
            }
            
            let projectId = try plotter.createProject(withName: name, description: description)
            print("✅ Created project '\(name)' with ID: \(projectId)")
        }
    }
    
    struct List: ParsableCommand {
        static let configuration = CommandConfiguration(
            abstract: "List all projects"
        )
        
        @OptionGroup var options: PlotterCLI
        
        func run() throws {
            guard let plotter = PLTPlotter(databasePath: options.database) else {
                throw CLIError.initializationFailed
            }
            
            let projects = try plotter.listProjects()
            
            if projects.isEmpty {
                print("No projects found.")
                return
            }
            
            print("Projects:")
            for project in projects {
                print("  • \(project.name) (\(project.identifier))")
                if !project.projectDescription.isEmpty {
                    print("    \(project.projectDescription)")
                }
            }
        }
    }
}

extension PlotterCLI {
    struct Folder: ParsableCommand {
        static let configuration = CommandConfiguration(
            commandName: "folder",
            abstract: "Manage folders within projects",
            subcommands: [Create.self, List.self]
        )
        
        @OptionGroup var options: PlotterCLI
    }
}

extension PlotterCLI.Folder {
    struct Create: ParsableCommand {
        static let configuration = CommandConfiguration(
            abstract: "Create a new folder in a project"
        )
        
        @OptionGroup var options: PlotterCLI
        
        @Option(name: .shortAndLong, help: "The folder name")
        var name: String
        
        @Option(name: .shortAndLong, help: "The folder description")
        var description: String = ""
        
        @Option(name: .shortAndLong, help: "Parent project ID")
        var projectId: String
        
        @Option(name: [.customLong("parent-folder")], help: "Parent folder ID (for subfolders)")
        var parentFolderId: String?
        
        func run() throws {
            guard let plotter = PLTPlotter(databasePath: options.database) else {
                throw CLIError.initializationFailed
            }
            
            let folderId = try plotter.createFolder(withName: name, 
                                                   description: description,
                                                   parentProjectId: projectId,
                                                   parentFolderId: parentFolderId)
            print("✅ Created folder '\(name)' with ID: \(folderId)")
        }
    }
    
    struct List: ParsableCommand {
        static let configuration = CommandConfiguration(
            abstract: "List all folders in a project"
        )
        
        @OptionGroup var options: PlotterCLI
        
        @Option(name: .shortAndLong, help: "Project ID")
        var projectId: String
        
        func run() throws {
            guard let plotter = PLTPlotter(databasePath: options.database) else {
                throw CLIError.initializationFailed
            }
            
            let folders = try plotter.listFolders(inProject: projectId)
            
            if folders.isEmpty {
                print("No folders found in this project.")
                return
            }
            
            print("Folders in project \(projectId):")
            for folder in folders {
                print("  • \(folder.name) (\(folder.identifier))")
                if !folder.folderDescription.isEmpty {
                    print("    \(folder.folderDescription)")
                }
            }
        }
    }
}

extension PlotterCLI {
    struct Note: ParsableCommand {
        static let configuration = CommandConfiguration(
            commandName: "note",
            abstract: "Manage notes within folders",
            subcommands: [Create.self, List.self, View.self, Edit.self]
        )
        
        @OptionGroup var options: PlotterCLI
    }
}

extension PlotterCLI.Note {
    struct Create: ParsableCommand {
        static let configuration = CommandConfiguration(
            abstract: "Create a new note in a folder"
        )
        
        @OptionGroup var options: PlotterCLI
        
        @Option(name: .shortAndLong, help: "The note name")
        var name: String
        
        @Option(name: .shortAndLong, help: "Parent folder ID")
        var folderId: String
        
        @Option(name: .shortAndLong, help: "Initial note content")
        var content: String = ""
        
        func run() throws {
            guard let plotter = PLTPlotter(databasePath: options.database) else {
                throw CLIError.initializationFailed
            }
            
            let noteId = try plotter.createNote(withName: name,
                                               parentFolderId: folderId,
                                               initialContent: content)
            print("✅ Created note '\(name)' with ID: \(noteId)")
        }
    }
    
    struct List: ParsableCommand {
        static let configuration = CommandConfiguration(
            abstract: "List all notes in a folder"
        )
        
        @OptionGroup var options: PlotterCLI
        
        @Option(name: .shortAndLong, help: "Folder ID")
        var folderId: String
        
        func run() throws {
            guard let plotter = PLTPlotter(databasePath: options.database) else {
                throw CLIError.initializationFailed
            }
            
            let notes = try plotter.listNotes(inFolder: folderId)
            
            if notes.isEmpty {
                print("No notes found in this folder.")
                return
            }
            
            print("Notes in folder \(folderId):")
            for note in notes {
                print("  • \(note.name) (\(note.identifier))")
                print("    Path: \(note.path)")
            }
        }
    }
    
    struct View: ParsableCommand {
        static let configuration = CommandConfiguration(
            abstract: "View note content"
        )
        
        @OptionGroup var options: PlotterCLI
        
        @Option(name: .shortAndLong, help: "Note ID")
        var noteId: String
        
        func run() throws {
            guard let plotter = PLTPlotter(databasePath: options.database) else {
                throw CLIError.initializationFailed
            }
            
            let content = try plotter.getNoteContent(noteId)
            print(content)
        }
    }
    
    struct Edit: ParsableCommand {
        static let configuration = CommandConfiguration(
            abstract: "Edit note content"
        )
        
        @OptionGroup var options: PlotterCLI
        
        @Option(name: .shortAndLong, help: "Note ID")
        var noteId: String
        
        @Option(name: .shortAndLong, help: "New content")
        var content: String
        
        func run() throws {
            guard let plotter = PLTPlotter(databasePath: options.database) else {
                throw CLIError.initializationFailed
            }
            
            try plotter.updateNoteContent(noteId, content: content)
            print("✅ Updated note content")
        }
    }
}

extension PlotterCLI {
    struct Interactive: ParsableCommand {
        static let configuration = CommandConfiguration(
            commandName: "interactive",
            abstract: "Enter interactive mode for managing projects"
        )
        
        @OptionGroup var options: PlotterCLI
        
        func run() throws {
            let interactiveMode = try InteractiveMode(databasePath: options.database)
            interactiveMode.run()
        }
    }
}

enum CLIError: LocalizedError {
    case initializationFailed
    
    var errorDescription: String? {
        switch self {
        case .initializationFailed:
            return "Failed to initialize Plotter"
        }
    }
}
