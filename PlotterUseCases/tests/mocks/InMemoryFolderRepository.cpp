#include "InMemoryFolderRepository.h"
#include <algorithm>

std::string InMemoryFolderRepository::save(const Folder& folder) {
    storage.emplace(folder.getId(), folder);
    return folder.getId();
}

std::optional<Folder> InMemoryFolderRepository::findById(const std::string& id) {
    auto it = storage.find(id);
    if (it != storage.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Folder> InMemoryFolderRepository::findAll() {
    std::vector<Folder> result;
    result.reserve(storage.size());
    
    for (const auto& pair : storage) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::vector<Folder> InMemoryFolderRepository::findByParentProjectId(const std::string& parentProjectId) {
    std::vector<Folder> result;
    
    for (const auto& pair : storage) {
        if (pair.second.getParentProjectId() == parentProjectId) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<Folder> InMemoryFolderRepository::findByParentFolderId(const std::string& parentFolderId) {
    std::vector<Folder> result;
    
    for (const auto& pair : storage) {
        if (pair.second.getParentFolderId() == parentFolderId) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

bool InMemoryFolderRepository::deleteById(const std::string& id) {
    return storage.erase(id) > 0;
}

void InMemoryFolderRepository::update(const Folder& folder) {
    auto it = storage.find(folder.getId());
    if (it == storage.end()) {
        throw std::runtime_error("Folder not found for update: " + folder.getId());
    }
    
    it->second = folder;
}

bool InMemoryFolderRepository::exists(const std::string& id) {
    return storage.find(id) != storage.end();
}

void InMemoryFolderRepository::clear() {
    storage.clear();
}

size_t InMemoryFolderRepository::size() const {
    return storage.size();
}