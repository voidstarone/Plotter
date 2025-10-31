#include "Project.h"
#include <algorithm>

// Constructor
Project::Project(const std::string& id, const std::string& name, const std::string& description)
    : FileItem(id, name, "project"),
      description(description) {}

// Getters
const std::string& Project::getDescription() const {
    return description;
}

const std::vector<std::string>& Project::getFolderIds() const {
    return folderIds;
}

// Setters
void Project::setDescription(const std::string& description) {
    this->description = description;
}

// Folder ID management
void Project::addFolderId(const std::string& folderId) {
    folderIds.push_back(folderId);
}

bool Project::removeFolderId(const std::string& folderId) {
    auto it = std::find(folderIds.begin(), folderIds.end(), folderId);
    if (it != folderIds.end()) {
        folderIds.erase(it);
        return true;
    }
    return false;
}
