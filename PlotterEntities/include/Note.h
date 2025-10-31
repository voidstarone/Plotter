#ifndef NOTE_H
#define NOTE_H

#include "FileItem.h"
#include <string>
#include <unordered_map>
#include <chrono>
#include <any>

/**
 * @brief Represents a note entity with content and metadata
 * 
 * Note is a pure entity that stores its content, path, and parent folder ID.
 * It extends FileItem to inherit id, name, and type properties.
 * This entity has NO infrastructure dependencies - it's just data and business logic.
 */
class Note : public FileItem {
private:
    std::string path; // Path to note content in storage (metadata only)
    std::string content; // The actual content of the note
    std::string parentFolderId; // ID of parent folder (relationship)
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    std::unordered_map<std::string, std::any> attributes;

public:
    /**
     * @brief Constructor for Note entity
     * 
     * @param id Unique identifier for this note
     * @param name The name/title of the note
     * @param path The path where the note content is stored (for reference)
     * @param parentFolderId The ID of the parent folder (empty string if none)
     */
    Note(const std::string& id, const std::string& name, const std::string& path,
         const std::string& parentFolderId);

    /**
     * @brief Get the storage path
     * 
     * @return Const reference to the note's storage path
     */
    const std::string& getPath() const;

    /**
     * @brief Set the storage path
     * 
     * @param path The new storage path
     */
    void setPath(const std::string& path);

    /**
     * @brief Get the parent folder ID
     * 
     * @return Const reference to the parent folder's ID
     */
    const std::string& getParentFolderId() const;

    /**
     * @brief Set the parent folder ID
     * 
     * @param parentFolderId The ID of the parent folder
     */
    void setParentFolderId(const std::string& parentFolderId);
    
    /**
     * @brief Get the note content
     * 
     * @return Const reference to the note's content
     */
    const std::string& getContent() const;

    /**
     * @brief Set the note content
     * 
     * @param content The new content for the note
     */
    void setContent(const std::string& content);

    /**
     * @brief Get the creation timestamp
     * 
     * @return Const reference to the note's creation time point
     */
    const std::chrono::system_clock::time_point& getCreatedAt() const;
    
    /**
     * @brief Get the last modification timestamp
     * 
     * Returns the timestamp when this note was last modified. This includes
     * changes to title, content, or attributes.
     * 
     * @return Const reference to the note's last modification time point
     */
    /**
     * @brief Get the last modification timestamp
     * 
     * @return Const reference to the note's last modification time point
     */
    const std::chrono::system_clock::time_point& getUpdatedAt() const;

    /**
     * @brief Set an attribute with a specific type
     * 
     * @tparam T The type of the attribute value
     * @param key The attribute key
     * @param value The attribute value
     */
    template<typename T>
    void setAttribute(const std::string& key, const T& value);

    /**
     * @brief Get an attribute value
     * 
     * @tparam T The expected type of the attribute value
     * @param key The attribute key
     * @return The attribute value
     * @throws std::runtime_error if the attribute doesn't exist or type mismatch
     */
    template<typename T>
    T getAttribute(const std::string& key) const;

    /**
     * @brief Get an attribute value with a default
     * 
     * @tparam T The expected type of the attribute value
     * @param key The attribute key
     * @param defaultValue The default value if attribute doesn't exist
     * @return The attribute value or default
     */
    template<typename T>
    T getAttribute(const std::string& key, const T& defaultValue) const;

    /**
     * @brief Check if an attribute exists
     * 
     * @param key The attribute key
     * @return True if the attribute exists, false otherwise
     */
    bool hasAttribute(const std::string& key) const;

    /**
     * @brief Remove an attribute
     * 
     * @param key The attribute key
     * @return True if the attribute was found and removed, false otherwise
     */
    bool removeAttribute(const std::string& key);

private:
    void updateTimestamp();
};

// Template implementations
template<typename T>
void Note::setAttribute(const std::string& key, const T& value) {
    attributes[key] = value;
    updateTimestamp();
}

template<typename T>
T Note::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    if (it == attributes.end()) {
        throw std::runtime_error("Attribute not found: " + key);
    }
    
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        throw std::runtime_error("Type mismatch for attribute: " + key);
    }
}

template<typename T>
T Note::getAttribute(const std::string& key, const T& defaultValue) const {
    auto it = attributes.find(key);
    if (it == attributes.end()) {
        return defaultValue;
    }
    
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        return defaultValue;
    }
}

#endif // NOTE_H
