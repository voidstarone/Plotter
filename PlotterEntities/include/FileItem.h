#ifndef FILEITEM_H
#define FILEITEM_H

#include <string>

/**
 * @brief Base class for all file system items (notes, folders, projects)
 * 
 * FileItem provides common properties shared by all entities in the system:
 * a unique identifier, a human-readable name, and a type string.
 */
class FileItem {
private:
    std::string id;
    std::string name;
    std::string type;

public:
    /**
     * @brief Constructor with id, name, and type
     * 
     * @param id Unique identifier for this item
     * @param name Human-readable name
     * @param type Type string (e.g., "note", "folder", "project")
     */
    FileItem(const std::string& id, const std::string& name, const std::string& type);

    /**
     * @brief Virtual destructor for proper cleanup in derived classes
     */
    virtual ~FileItem() = default;

    /**
     * @brief Get the unique identifier
     * 
     * @return Const reference to the item's ID
     */
    const std::string& getId() const;

    /**
     * @brief Get the name
     * 
     * @return Const reference to the item's name
     */
    const std::string& getName() const;

    /**
     * @brief Get the type string
     * 
     * @return Const reference to the item's type
     */
    const std::string& getType() const;

    /**
     * @brief Set the name
     * 
     * @param name The new name for the item
     */
    void setName(const std::string& name);

    /**
     * @brief Set the type
     * 
     * @param type The new type string for the item
     */
    void setType(const std::string& type);
};

#endif // FILEITEM_H
