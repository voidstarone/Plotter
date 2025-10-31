#include "Folder.h"
#include <algorithm>

// Constructor
Folder::Folder(const std::string& id, const std::string& name, const std::string& description,
               const std::string& parentProjectId, const std::string& parentFolderId)
    : FileItem(id, name, "folder"),
      description(description),
      parentProjectId(parentProjectId),
      parentFolderId(parentFolderId) {}

// Getters
const std::string& Folder::getDescription() const {
    return description;
}

const std::string& Folder::getParentProjectId() const {
    return parentProjectId;
}

const std::string& Folder::getParentFolderId() const {
    return parentFolderId;
}

const std::vector<std::string>& Folder::getNoteIds() const {
    return noteIds;
}

const std::vector<std::string>& Folder::getSubfolderIds() const {
    return subfolderIds;
}

// Setters
void Folder::setDescription(const std::string& description) {
    this->description = description;
}

void Folder::setParentProjectId(const std::string& parentProjectId) {
    this->parentProjectId = parentProjectId;
}

void Folder::setParentFolderId(const std::string& parentFolderId) {
    this->parentFolderId = parentFolderId;
}

// Note ID management
void Folder::addNoteId(const std::string& noteId) {
    noteIds.push_back(noteId);
}

bool Folder::removeNoteId(const std::string& noteId) {
    auto it = std::find(noteIds.begin(), noteIds.end(), noteId);
    if (it != noteIds.end()) {
        noteIds.erase(it);
        return true;
    }
    return false;
}

// Subfolder ID management
void Folder::addSubfolderId(const std::string& subfolderId) {
    subfolderIds.push_back(subfolderId);
}

bool Folder::removeSubfolderId(const std::string& subfolderId) {
    auto it = std::find(subfolderIds.begin(), subfolderIds.end(), subfolderId);
    if (it != subfolderIds.end()) {
        subfolderIds.erase(it);
        return true;
    }
    return false;
}
