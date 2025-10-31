#include "InMemoryNoteRepository.h"
#include <algorithm>
#include <cctype>

InMemoryNoteRepository::InMemoryNoteRepository(std::shared_ptr<NoteStorage> storage)
    : noteStorage(storage) {
    if (!noteStorage) {
        throw std::invalid_argument("NoteStorage cannot be null");
    }
}

std::string InMemoryNoteRepository::save(const Note& note) {
    storage.emplace(note.getId(), note);
    return note.getId();
}

std::optional<Note> InMemoryNoteRepository::findById(const std::string& id) {
    auto it = storage.find(id);
    if (it != storage.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Note> InMemoryNoteRepository::findAll() {
    std::vector<Note> result;
    result.reserve(storage.size());
    
    for (const auto& pair : storage) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::vector<Note> InMemoryNoteRepository::findByParentFolderId(const std::string& parentFolderId) {
    std::vector<Note> result;
    
    for (const auto& pair : storage) {
        if (pair.second.getParentFolderId() == parentFolderId) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<Note> InMemoryNoteRepository::search(const std::string& searchTerm) {
    std::vector<Note> result;
    
    if (searchTerm.empty()) {
        return result;
    }
    
    // Convert search term to lowercase for case-insensitive search
    std::string lowerSearchTerm = searchTerm;
    std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerSearchTerm.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    for (const auto& pair : storage) {
        const Note& note = pair.second;
        
        // Search in note name
        std::string lowerName = note.getName();
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        
        bool foundInName = lowerName.find(lowerSearchTerm) != std::string::npos;
        bool foundInContent = false;
        
        // Search in content if not found in name
        if (!foundInName) {
            try {
                std::string content = note.getContent();
                std::transform(content.begin(), content.end(), content.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                foundInContent = content.find(lowerSearchTerm) != std::string::npos;
            } catch (const std::exception&) {
                // If content can't be loaded, skip content search
                foundInContent = false;
            }
        }
        
        if (foundInName || foundInContent) {
            result.push_back(note);
        }
    }
    
    return result;
}

bool InMemoryNoteRepository::deleteById(const std::string& id) {
    return storage.erase(id) > 0;
}

void InMemoryNoteRepository::update(const Note& note) {
    auto it = storage.find(note.getId());
    if (it == storage.end()) {
        throw std::runtime_error("Note not found for update: " + note.getId());
    }
    
    it->second = note;
}

bool InMemoryNoteRepository::exists(const std::string& id) {
    return storage.find(id) != storage.end();
}

void InMemoryNoteRepository::clear() {
    storage.clear();
}

size_t InMemoryNoteRepository::size() const {
    return storage.size();
}

std::shared_ptr<NoteStorage> InMemoryNoteRepository::getNoteStorage() const {
    return noteStorage;
}