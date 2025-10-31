#include "InMemoryProjectRepository.h"
#include <algorithm>

std::string InMemoryProjectRepository::save(const Project& project) {
    storage.emplace(project.getId(), project);
    return project.getId();
}

std::optional<Project> InMemoryProjectRepository::findById(const std::string& id) {
    auto it = storage.find(id);
    if (it != storage.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Project> InMemoryProjectRepository::findAll() {
    std::vector<Project> result;
    result.reserve(storage.size());
    
    for (const auto& pair : storage) {
        result.push_back(pair.second);
    }
    
    return result;
}

bool InMemoryProjectRepository::deleteById(const std::string& id) {
    return storage.erase(id) > 0;
}

void InMemoryProjectRepository::update(const Project& project) {
    auto it = storage.find(project.getId());
    if (it == storage.end()) {
        throw std::runtime_error("Project not found for update: " + project.getId());
    }
    
    it->second = project;
}

bool InMemoryProjectRepository::exists(const std::string& id) {
    return storage.find(id) != storage.end();
}

void InMemoryProjectRepository::clear() {
    storage.clear();
}

size_t InMemoryProjectRepository::size() const {
    return storage.size();
}