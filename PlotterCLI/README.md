# Plotter CLI

A Swift command-line tool for managing notes with projects and folders, built on top of the Plotter C++ libraries.

## Architecture

```
Swift CLI (PlotterCLI.swift)
        ↓
Objective-C++ Bridge (PlotterObjCBridge.mm)
        ↓
C++ Use Cases & Repositories
        ↓
SQLite Database
```

**Why Objective-C++?**
- Swift can't directly call C++, but has perfect interop with Objective-C
- Objective-C++ (.mm files) can directly use C++ classes
- Much cleaner than a C bridge layer - no manual memory management or marshalling

## Building

### Prerequisites
- macOS with Xcode
- Swift 5.9+
- C++17 compiler
- All Plotter C++ libraries built

### Build the C++ libraries first

```bash
# Build all C++ dependencies
cd PlotterEntities/build && cmake .. && make && cd ../..
cd PlotterDTOs/build && cmake .. && make && cd ../..
cd PlotterSqliteDTOs/build && cmake .. && make && cd ../..
cd PlotterRepositories/build && cmake .. && make && cd ../..
cd PlotterSqliteMappers/build && cmake .. && make && cd ../..
cd PlotterSqliteDataSource/build && cmake .. && make && cd ../..
cd PlotterUseCases/build && cmake .. && make && cd ../..
cd PlotterLogger/build && cmake .. && make && cd ../..
```

### Build the Swift CLI

```bash
cd PlotterCLI
swift build -c release
```

## Usage

### Command-Line Mode

The `-d` or `--database` argument is **required** for all commands:

```bash
# Create a project
plotter -d mydb.db project create -n "My Project" -d "Project description"

# List all projects
plotter -d mydb.db project list
```

### Interactive Mode

Launch the terminal-based UI for easier navigation:

```bash
plotter -d mydb.db interactive
```

**Interactive Features:**
- 📋 List all projects
- ➕ Create new project
- 🔍 View project details
- 📊 Show statistics
- 🗑️ Delete project (coming soon)

**Navigation:**
- Enter a number to select a menu option
- Follow the prompts to enter data
- Press Enter to continue after viewing results
- Select `0` to exit

## Database

The database path is **required** via the `-d` or `--database` argument:

```bash
# Relative path (creates in current directory)
plotter -d notes.db interactive

# Absolute path
plotter -d /path/to/my/database.db project list
```

**Auto-Creation:**
- If the database file doesn't exist, it will be created automatically
- Schema is initialized on first connection
- Both absolute and relative paths are supported

## Features

- ✅ Create and list projects (command-line and interactive)
- ✅ Interactive terminal UI with menus
- ✅ SQLite persistence with automatic database creation
- ✅ Clean architecture with C++ use cases and repositories
- ✅ Type-safe Swift interface via Objective-C++ bridge
- 🔄 Folder management (in progress)
- 🔄 Note management (in progress)
- 🔄 Project deletion (in progress)

## Example Workflow

### Command-Line
```bash
# Create multiple projects
plotter -d work.db project create -n "Website Redesign" -d "Update company website"
plotter -d work.db project create -n "Mobile App" -d "iOS and Android apps"

# List all projects
plotter -d work.db project list
```

### Interactive Mode
```bash
# Launch interactive UI
plotter -d work.db interactive

# Then use the menu:
# 1 - See all projects
# 2 - Create a new project
# 3 - View project details
# 5 - See statistics
# 0 - Exit
```
