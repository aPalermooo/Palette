/******************************************
*   Name:           DirectoryHandler.cpp
*   Description:    Declaration - Class responsible for servicing the front end and handing interactions with Directories
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu> <Primary>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#include <windows.h>

#include <explorer/DirectoryHandler.h>

int DirectoryHandler::create(const std::filesystem::path &current, const std::string &name) {
    auto path = current / name;
    if (std::filesystem::is_directory(path))
        return 1;
    return std::filesystem::create_directory(path) ? 0 : -1;
}

int DirectoryHandler::remove(const std::filesystem::path &path) {
    if (!std::filesystem::is_directory(path)) return 1;
    return std::filesystem::remove(path) ? 0 : -1;
}

void DirectoryHandler::open(const std::filesystem::path &path) {
    if (!std::filesystem::is_directory(path))
        ShellExecuteA(nullptr, "explore", path.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
}

std::vector<std::filesystem::directory_entry> DirectoryHandler::list(const std::filesystem::path &path) {
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(path))
        if (entry.is_directory())
            entries.push_back(entry);
    return entries;
}

int DirectoryHandler::forceRemove(const std::filesystem::path &path) {
    if (!std::filesystem::is_directory(path)) return -1;
    std::error_code ec;
    const int filesDeleted = std::filesystem::remove_all(path, ec);
    return ec ? -2 : filesDeleted;
}

std::filesystem::path DirectoryHandler::descend(const std::filesystem::path &current, const std::string& target) {
    auto child = current / target;
    return std::filesystem::is_directory(child) ? child : current;
}

std::filesystem::path DirectoryHandler::ascend(const std::filesystem::path &current) {
    // Add guard to leaving palette folder
    auto parent = current.parent_path();
    return parent == current ? current : parent;
}