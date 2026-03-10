/******************************************
*   Name:           FileHandler.cpp
*   Description:    Implementation - Class responsible for servicing the front end and handing interactions with files
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu> <Primary>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#include <windows.h>
#include <fstream>

#include "explorer/FileHandler.h"

int FileHandler::create(const std::filesystem::path& current, const std::string& name) {
    return FileHandler::create(current, name, "");
}

int FileHandler::create(const std::filesystem::path& current, const std::string& name, const std::string& extension) {
    const auto path = current / (extension.empty() ? name : name + "." + extension);
    if (std::filesystem::exists(path)) return 1;
    return forceCreate(current, name, extension);
}

int FileHandler::remove(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) return 1;
    return std::filesystem::remove(path) ? 0 : -1;
}

void FileHandler::open(const std::filesystem::path& path) {
    if (!std::filesystem::is_directory(path))
        ShellExecuteA(nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
}

std::vector<std::filesystem::directory_entry> FileHandler::list(const std::filesystem::path& path) {
    std::vector<std::filesystem::directory_entry> entries;
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) //directory does not exist
        return entries;

    for (const auto& entry : std::filesystem::directory_iterator(path))
        if (entry.is_regular_file())
            entries.push_back(entry);
    return entries;
}

int FileHandler::forceCreate(const std::filesystem::path& current, const std::string& name, const std::string& extension) {
    const auto path = current / (extension.empty() ? name : name + "." + extension);
    const std::ofstream file(path);
    return file.good() ? 0 : -1;
}
