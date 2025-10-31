#import "PlotterObjCBridge.h"

// C++ includes - Entities
#include "Project.h"
#include "Folder.h"
#include "Note.h"

// C++ includes - Use cases
#include "usecases/CreateProjectUseCase.h"
#include "usecases/ListProjectsUseCase.h"
#include "usecases/CreateFolderUseCase.h"
#include "usecases/CreateNoteUseCase.h"
#include "usecases/GetNoteContentUseCase.h"

// C++ includes - Repositories
#include "plotter_repositories/MultiSourceProjectRepository.h"
#include "plotter_repositories/MultiSourceFolderRepository.h"
#include "plotter_repositories/MultiSourceNoteRepository.h"

// C++ includes - Data sources
#include "plotter_datasource_router/SimpleDataSourceRouter.h"
#include "plotter_sqlite/SqliteProjectDataSource.h"
#include "plotter_sqlite/SqliteFolderDataSource.h"
#include "plotter_sqlite/SqliteNoteDataSource.h"

// C++ includes - Mappers
#include "plotter_sqlite_mappers/SqliteMappers.h"

#include <memory>

using namespace plotter;
using namespace plotter::repositories;
using namespace plotter::sqlite;
using namespace plotter::sqlite_mappers;
using namespace plotter::datasource_router;

static NSString *const PLTErrorDomain = @"com.plotter.error";

@implementation PLTProject

- (instancetype)initWithCppProject:(const Project&)project {
    if (self = [super init]) {
        _identifier = [NSString stringWithUTF8String:project.getId().c_str()];
        _name = [NSString stringWithUTF8String:project.getName().c_str()];
        _projectDescription = [NSString stringWithUTF8String:project.getDescription().c_str()];
    }
    return self;
}

@end

@implementation PLTFolder

- (instancetype)initWithCppFolder:(const Folder&)folder {
    if (self = [super init]) {
        _identifier = [NSString stringWithUTF8String:folder.getId().c_str()];
        _name = [NSString stringWithUTF8String:folder.getName().c_str()];
        _folderDescription = [NSString stringWithUTF8String:folder.getDescription().c_str()];
        _parentProjectId = [NSString stringWithUTF8String:folder.getParentProjectId().c_str()];
        
        const auto& parentFolderId = folder.getParentFolderId();
        if (!parentFolderId.empty()) {
            _parentFolderId = [NSString stringWithUTF8String:parentFolderId.c_str()];
        } else {
            _parentFolderId = nil;
        }
    }
    return self;
}

@end

@implementation PLTNote

- (instancetype)initWithCppNote:(const Note&)note {
    if (self = [super init]) {
        _identifier = [NSString stringWithUTF8String:note.getId().c_str()];
        _name = [NSString stringWithUTF8String:note.getName().c_str()];
        _path = [NSString stringWithUTF8String:note.getPath().c_str()];
        _parentFolderId = [NSString stringWithUTF8String:note.getParentFolderId().c_str()];
    }
    return self;
}

@end

@implementation PLTPlotter {
    // Data sources
    std::unique_ptr<SqliteProjectDataSource> _projectDS;
    std::unique_ptr<SqliteFolderDataSource> _folderDS;
    std::unique_ptr<SqliteNoteDataSource> _noteDS;
    
    // Mappers
    std::unique_ptr<SqliteProjectMapper> _projectMapper;
    std::unique_ptr<SqliteFolderMapper> _folderMapper;
    std::unique_ptr<SqliteNoteMapper> _noteMapper;
    
    // Routers
    std::unique_ptr<SimpleDataSourceRouter<ProjectDataSource>> _projectRouter;
    std::unique_ptr<SimpleDataSourceRouter<FolderDataSource>> _folderRouter;
    std::unique_ptr<SimpleDataSourceRouter<NoteDataSource>> _noteRouter;
    
    // Repositories
    std::shared_ptr<MultiSourceProjectRepository<SimpleDataSourceRouter<ProjectDataSource>>> _projectRepo;
    std::shared_ptr<MultiSourceFolderRepository<SimpleDataSourceRouter<FolderDataSource>>> _folderRepo;
    std::shared_ptr<MultiSourceNoteRepository<SimpleDataSourceRouter<NoteDataSource>>> _noteRepo;
    
    // Use cases
    std::unique_ptr<CreateProjectUseCase> _createProjectUC;
    std::unique_ptr<ListProjectsUseCase> _listProjectsUC;
    std::unique_ptr<CreateFolderUseCase> _createFolderUC;
    std::unique_ptr<CreateNoteUseCase> _createNoteUC;
    std::unique_ptr<GetNoteContentUseCase> _getNoteContentUC;
}

- (instancetype)initWithDatabasePath:(NSString *)dbPath {
    if (self = [super init]) {
        const char *path = [dbPath UTF8String];
        
        try {
            // Create data sources
            _projectDS = std::make_unique<SqliteProjectDataSource>("sqlite-project", path);
            _projectDS->connect();
            
            _folderDS = std::make_unique<SqliteFolderDataSource>("sqlite-folder", path);
            _folderDS->connect();
            
            _noteDS = std::make_unique<SqliteNoteDataSource>("sqlite-note", path);
            _noteDS->connect();
            
            // Create mappers
            _projectMapper = std::make_unique<SqliteProjectMapper>();
            _folderMapper = std::make_unique<SqliteFolderMapper>();
            _noteMapper = std::make_unique<SqliteNoteMapper>();
            
            // Create routers
            _projectRouter = std::make_unique<SimpleDataSourceRouter<ProjectDataSource>>(_projectDS.get());
            _folderRouter = std::make_unique<SimpleDataSourceRouter<FolderDataSource>>(_folderDS.get());
            _noteRouter = std::make_unique<SimpleDataSourceRouter<NoteDataSource>>(_noteDS.get());
            
            // Create repositories
            _projectRepo = std::make_shared<MultiSourceProjectRepository<SimpleDataSourceRouter<ProjectDataSource>>>(
                _projectRouter.get(), _projectMapper.get()
            );
            
            _folderRepo = std::make_shared<MultiSourceFolderRepository<SimpleDataSourceRouter<FolderDataSource>>>(
                _folderRouter.get(), _folderMapper.get()
            );
            
            _noteRepo = std::make_shared<MultiSourceNoteRepository<SimpleDataSourceRouter<NoteDataSource>>>(
                _noteRouter.get(), _noteMapper.get()
            );
            
            // Create use cases
            _createProjectUC = std::make_unique<CreateProjectUseCase>(_projectRepo);
            _listProjectsUC = std::make_unique<ListProjectsUseCase>(_projectRepo);
            _createFolderUC = std::make_unique<CreateFolderUseCase>(_folderRepo, _projectRepo);
            _createNoteUC = std::make_unique<CreateNoteUseCase>(_noteRepo, _folderRepo);
            _getNoteContentUC = std::make_unique<GetNoteContentUseCase>(_noteRepo);
            
        } catch (const std::exception &e) {
            NSLog(@"Failed to initialize Plotter: %s", e.what());
            return nil;
        }
    }
    return self;
}

- (nullable NSString *)createProjectWithName:(NSString *)name 
                                  description:(NSString *)description 
                                        error:(NSError **)error {
    try {
        CreateProjectUseCase::Request request;
        request.name = std::string([name UTF8String]);
        request.description = std::string([description UTF8String]);
        
        auto response = _createProjectUC->execute(request);
        
        if (response.success) {
            return [NSString stringWithUTF8String:response.result.projectId.c_str()];
        } else {
            if (error) {
                *error = [NSError errorWithDomain:PLTErrorDomain 
                                             code:-1 
                                         userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:response.error.message.c_str()]}];
            }
            return nil;
        }
    } catch (const std::exception &e) {
        if (error) {
            *error = [NSError errorWithDomain:PLTErrorDomain 
                                         code:-1 
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()]}];
        }
        return nil;
    }
}

- (nullable NSArray<PLTProject *> *)listProjectsWithError:(NSError **)error {
    try {
        ListProjectsUseCase::Request request;
        
        auto response = _listProjectsUC->execute(request);
        
        if (response.success) {
            NSMutableArray<PLTProject *> *projects = [NSMutableArray array];
            for (const auto& project : response.projects) {
                PLTProject *pltProject = [[PLTProject alloc] initWithCppProject:project];
                [projects addObject:pltProject];
            }
            return [projects copy];
        } else {
            if (error) {
                *error = [NSError errorWithDomain:PLTErrorDomain 
                                             code:-1 
                                         userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:response.errorMessage.c_str()]}];
            }
            return nil;
        }
    } catch (const std::exception &e) {
        if (error) {
            *error = [NSError errorWithDomain:PLTErrorDomain 
                                         code:-1 
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()]}];
        }
        return nil;
    }
}

// MARK: - Folder operations

- (nullable NSString *)createFolderWithName:(NSString *)name
                                 description:(NSString *)description
                            parentProjectId:(NSString *)projectId
                             parentFolderId:(nullable NSString *)folderId
                                      error:(NSError **)error {
    try {
        CreateFolderUseCase::Request request;
        request.name = std::string([name UTF8String]);
        request.description = std::string([description UTF8String]);
        request.parentProjectId = std::string([projectId UTF8String]);
        if (folderId) {
            request.parentFolderId = std::string([folderId UTF8String]);
        }
        
        auto response = _createFolderUC->execute(request);
        
        if (response.success) {
            return [NSString stringWithUTF8String:response.folderId.c_str()];
        } else {
            if (error) {
                *error = [NSError errorWithDomain:PLTErrorDomain 
                                             code:-1 
                                         userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:response.errorMessage.c_str()]}];
            }
            return nil;
        }
    } catch (const std::exception &e) {
        if (error) {
            *error = [NSError errorWithDomain:PLTErrorDomain 
                                         code:-1 
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()]}];
        }
        return nil;
    }
}

- (nullable NSArray<PLTFolder *> *)listFoldersInProject:(NSString *)projectId
                                                  error:(NSError **)error {
    try {
        // Get project to access its folder IDs
        auto project = _projectRepo->findById(std::string([projectId UTF8String]));
        if (!project.has_value()) {
            if (error) {
                *error = [NSError errorWithDomain:PLTErrorDomain 
                                             code:-1 
                                         userInfo:@{NSLocalizedDescriptionKey: @"Project not found"}];
            }
            return nil;
        }
        
        NSMutableArray<PLTFolder *> *folders = [NSMutableArray array];
        for (const auto& folderId : project->getFolderIds()) {
            auto folder = _folderRepo->findById(folderId);
            if (folder.has_value()) {
                PLTFolder *pltFolder = [[PLTFolder alloc] initWithCppFolder:*folder];
                [folders addObject:pltFolder];
            }
        }
        
        return [folders copy];
    } catch (const std::exception &e) {
        if (error) {
            *error = [NSError errorWithDomain:PLTErrorDomain 
                                         code:-1 
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()]}];
        }
        return nil;
    }
}

// MARK: - Note operations

- (nullable NSString *)createNoteWithName:(NSString *)name
                          parentFolderId:(NSString *)folderId
                          initialContent:(NSString *)content
                                   error:(NSError **)error {
    try {
        CreateNoteUseCase::Request request;
        request.name = std::string([name UTF8String]);
        request.parentFolderId = std::string([folderId UTF8String]);
        request.initialContent = std::string([content UTF8String]);
        
        auto response = _createNoteUC->execute(request);
        
        if (response.success) {
            return [NSString stringWithUTF8String:response.noteId.c_str()];
        } else {
            if (error) {
                *error = [NSError errorWithDomain:PLTErrorDomain 
                                             code:-1 
                                         userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:response.errorMessage.c_str()]}];
            }
            return nil;
        }
    } catch (const std::exception &e) {
        if (error) {
            *error = [NSError errorWithDomain:PLTErrorDomain 
                                         code:-1 
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()]}];
        }
        return nil;
    }
}

- (nullable NSArray<PLTNote *> *)listNotesInFolder:(NSString *)folderId
                                             error:(NSError **)error {
    try {
        // Get folder to access its note IDs
        auto folder = _folderRepo->findById(std::string([folderId UTF8String]));
        if (!folder.has_value()) {
            if (error) {
                *error = [NSError errorWithDomain:PLTErrorDomain 
                                             code:-1 
                                         userInfo:@{NSLocalizedDescriptionKey: @"Folder not found"}];
            }
            return nil;
        }
        
        NSMutableArray<PLTNote *> *notes = [NSMutableArray array];
        for (const auto& noteId : folder->getNoteIds()) {
            auto note = _noteRepo->findById(noteId);
            if (note.has_value()) {
                PLTNote *pltNote = [[PLTNote alloc] initWithCppNote:*note];
                [notes addObject:pltNote];
            }
        }
        
        return [notes copy];
    } catch (const std::exception &e) {
        if (error) {
            *error = [NSError errorWithDomain:PLTErrorDomain 
                                         code:-1 
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()]}];
        }
        return nil;
    }
}

- (nullable NSString *)getNoteContent:(NSString *)noteId
                                error:(NSError **)error {
    try {
        GetNoteContentUseCase::Request request;
        request.noteId = std::string([noteId UTF8String]);
        
        auto response = _getNoteContentUC->execute(request);
        
        if (response.success) {
            return [NSString stringWithUTF8String:response.content.c_str()];
        } else {
            if (error) {
                *error = [NSError errorWithDomain:PLTErrorDomain 
                                             code:-1 
                                         userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:response.errorMessage.c_str()]}];
            }
            return nil;
        }
    } catch (const std::exception &e) {
        if (error) {
            *error = [NSError errorWithDomain:PLTErrorDomain 
                                         code:-1 
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()]}];
        }
        return nil;
    }
}

- (BOOL)updateNoteContent:(NSString *)noteId
                  content:(NSString *)content
                    error:(NSError **)error {
    try {
        // Get the note
        auto note = _noteRepo->findById(std::string([noteId UTF8String]));
        if (!note.has_value()) {
            if (error) {
                *error = [NSError errorWithDomain:PLTErrorDomain 
                                             code:-1 
                                         userInfo:@{NSLocalizedDescriptionKey: @"Note not found"}];
            }
            return NO;
        }
        
        // Update content
        note->setContent(std::string([content UTF8String]));
        
        // Save
        _noteRepo->update(*note);
        
        return YES;
    } catch (const std::exception &e) {
        if (error) {
            *error = [NSError errorWithDomain:PLTErrorDomain 
                                         code:-1 
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()]}];
        }
        return NO;
    }
}

@end
