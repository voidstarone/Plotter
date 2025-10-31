#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface PLTProject : NSObject
@property (nonatomic, copy, readonly) NSString *identifier;
@property (nonatomic, copy, readonly) NSString *name;
@property (nonatomic, copy, readonly) NSString *projectDescription;
@end

@interface PLTFolder : NSObject
@property (nonatomic, copy, readonly) NSString *identifier;
@property (nonatomic, copy, readonly) NSString *name;
@property (nonatomic, copy, readonly) NSString *folderDescription;
@property (nonatomic, copy, readonly) NSString *parentProjectId;
@property (nonatomic, copy, readonly, nullable) NSString *parentFolderId;
@end

@interface PLTNote : NSObject
@property (nonatomic, copy, readonly) NSString *identifier;
@property (nonatomic, copy, readonly) NSString *name;
@property (nonatomic, copy, readonly) NSString *path;
@property (nonatomic, copy, readonly) NSString *parentFolderId;
@end

@interface PLTPlotter : NSObject

- (nullable instancetype)initWithDatabasePath:(NSString *)dbPath;

// Project operations
- (nullable NSString *)createProjectWithName:(NSString *)name 
                                  description:(NSString *)description 
                                        error:(NSError * _Nullable * _Nullable)error;
- (nullable NSArray<PLTProject *> *)listProjectsWithError:(NSError * _Nullable * _Nullable)error;

// Folder operations
- (nullable NSString *)createFolderWithName:(NSString *)name
                                 description:(NSString *)description
                            parentProjectId:(NSString *)projectId
                             parentFolderId:(nullable NSString *)folderId
                                      error:(NSError * _Nullable * _Nullable)error;
- (nullable NSArray<PLTFolder *> *)listFoldersInProject:(NSString *)projectId
                                                  error:(NSError * _Nullable * _Nullable)error;

// Note operations
- (nullable NSString *)createNoteWithName:(NSString *)name
                          parentFolderId:(NSString *)folderId
                          initialContent:(NSString *)content
                                   error:(NSError * _Nullable * _Nullable)error;
- (nullable NSArray<PLTNote *> *)listNotesInFolder:(NSString *)folderId
                                             error:(NSError * _Nullable * _Nullable)error;
- (nullable NSString *)getNoteContent:(NSString *)noteId
                                error:(NSError * _Nullable * _Nullable)error;
- (BOOL)updateNoteContent:(NSString *)noteId
                  content:(NSString *)content
                    error:(NSError * _Nullable * _Nullable)error;

@end

NS_ASSUME_NONNULL_END
