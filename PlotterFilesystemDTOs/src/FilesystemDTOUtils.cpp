#include "plotter_filesystem_dtos/FilesystemDTOs.h"
#include <chrono>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <sys/stat.h>

namespace plotter {
namespace filesystem_dtos {

long long FilesystemDTOUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

std::string FilesystemDTOUtils::generateId() {
    // Generate a simple UUID-like string
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    uint64_t part1 = dis(gen);
    uint64_t part2 = dis(gen);

    oss << std::setw(8) << (part1 >> 32) << "-"
        << std::setw(4) << ((part1 >> 16) & 0xFFFF) << "-"
        << std::setw(4) << (part1 & 0xFFFF) << "-"
        << std::setw(4) << (part2 >> 48) << "-"
        << std::setw(12) << (part2 & 0xFFFFFFFFFFFF);

    return oss.str();
}

std::string FilesystemDTOUtils::readDotfile(const std::string& dotfilePath) {
    std::ifstream file(dotfilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open dotfile: " + dotfilePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FilesystemDTOUtils::writeDotfile(const std::string& dotfilePath, const std::string& content) {
    std::ofstream file(dotfilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to write dotfile: " + dotfilePath);
    }

    file << content;
    file.close();
}

bool FilesystemDTOUtils::isProjectDirectory(const std::string& path) {
    struct stat st;
    std::string dotfile = path + "/.plotter_project";
    return (stat(dotfile.c_str(), &st) == 0);
}

bool FilesystemDTOUtils::isFolderDirectory(const std::string& path) {
    struct stat st;
    std::string dotfile = path + "/.plotter_folder";
    return (stat(dotfile.c_str(), &st) == 0);
}

bool FilesystemDTOUtils::isNoteFile(const std::string& path) {
    struct stat st;
    std::string metafile = path + ".plotter_meta";
    return (stat(metafile.c_str(), &st) == 0);
}

} // namespace filesystem_dtos
} // namespace plotter

