#include "FileItem.h"

// Constructor
FileItem::FileItem(const std::string& id, const std::string& name, const std::string& type)
    : id(id), name(name), type(type) {}

// Getters
const std::string& FileItem::getId() const {
    return id;
}

const std::string& FileItem::getName() const {
    return name;
}

const std::string& FileItem::getType() const {
    return type;
}

// Setters
void FileItem::setName(const std::string& name) {
    this->name = name;
}

void FileItem::setType(const std::string& type) {
    this->type = type;
}
