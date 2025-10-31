#include "Note.h"
#include <algorithm>
#include <iomanip>
#include <vector>

// Constructor
Note::Note(const std::string& id, const std::string& name, const std::string& path,
           const std::string& parentFolderId)
    : FileItem(id, name, "note"),
      path(path),
      content(""),
      parentFolderId(parentFolderId),
      createdAt(std::chrono::system_clock::now()), 
      updatedAt(std::chrono::system_clock::now()) {}

// Getters
const std::string& Note::getPath() const {
    return path;
}

const std::string& Note::getParentFolderId() const {
    return parentFolderId;
}

const std::string& Note::getContent() const {
    return content;
}

const std::chrono::system_clock::time_point& Note::getCreatedAt() const {
    return createdAt;
}

const std::chrono::system_clock::time_point& Note::getUpdatedAt() const {
    return updatedAt;
}

// Setters
void Note::setPath(const std::string& path) {
    this->path = path;
    updateTimestamp();
}

void Note::setParentFolderId(const std::string& parentFolderId) {
    this->parentFolderId = parentFolderId;
    updateTimestamp();
}

void Note::setContent(const std::string& content) {
    this->content = content;
    updateTimestamp();
}

// Attribute management
bool Note::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

bool Note::removeAttribute(const std::string& key) {
    auto it = attributes.find(key);
    if (it != attributes.end()) {
        attributes.erase(it);
        updateTimestamp();
        return true;
    }
    return false;
}

// Private methods
void Note::updateTimestamp() {
    updatedAt = std::chrono::system_clock::now();
}
