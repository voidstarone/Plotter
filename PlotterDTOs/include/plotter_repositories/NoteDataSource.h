// This file includes all data source stubs
#include "ProjectDataSource.h"
#ifndef PLOTTER_DATASOURCE_STUBS_H
#define PLOTTER_DATASOURCE_STUBS_H

/**
 * @file DataSourceStubs.h
 * @brief Minimal data source interface stubs for backend-only builds
 *
 * These are stub interfaces to allow data source implementations to compile independently.
 * When integrating with the full application, link against the actual repository library.
 */

#include "BaseDTOs.h"
#include <string>
#include <vector>

namespace plotter {
namespace repositories {

/**
 * @brief Base interface for Project data sources
 */
class ProjectDataSource {
public:
    virtual ~ProjectDataSource() = default;

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual std::string getName() const = 0;

    virtual std::string create(dto::ProjectDTO* dto) = 0;
    virtual dto::ProjectDTO* read(const std::string& id) = 0;
    virtual bool update(const std::string& id, dto::ProjectDTO* dto) = 0;
    virtual bool remove(const std::string& id) = 0;
    virtual std::vector<dto::ProjectDTO*> list() = 0;
};

/**
 * @brief Base interface for Folder data sources
 */
class FolderDataSource {
public:
    virtual ~FolderDataSource() = default;

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual std::string getName() const = 0;

    virtual std::string create(dto::FolderDTO* dto) = 0;
    virtual dto::FolderDTO* read(const std::string& id) = 0;
    virtual bool update(const std::string& id, dto::FolderDTO* dto) = 0;
    virtual bool remove(const std::string& id) = 0;
    virtual std::vector<dto::FolderDTO*> listByProject(const std::string& projectId) = 0;
    virtual std::vector<dto::FolderDTO*> listByParentFolder(const std::string& folderId) = 0;
};

/**
 * @brief Base interface for Note data sources
 */
class NoteDataSource {
public:
    virtual ~NoteDataSource() = default;

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual std::string getName() const = 0;

    virtual std::string create(dto::NoteDTO* dto) = 0;
    virtual dto::NoteDTO* read(const std::string& id) = 0;
    virtual bool update(const std::string& id, dto::NoteDTO* dto) = 0;
    virtual bool remove(const std::string& id) = 0;
    virtual std::vector<dto::NoteDTO*> listByFolder(const std::string& folderId) = 0;

    virtual std::string getContent(const std::string& id) = 0;
    virtual bool updateContent(const std::string& id, const std::string& content) = 0;
};

} // namespace repositories
} // namespace plotter

#endif // PLOTTER_DATASOURCE_STUBS_H

