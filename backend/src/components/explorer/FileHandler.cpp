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
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) return 1;
    return std::filesystem::remove(path) ? 0 : -1;
}

int FileHandler::open(const std::filesystem::path &path) {
    // Validate existence of file
    if (!std::filesystem::exists(path.parent_path())) return -2;
    if (!std::filesystem::exists(path)) return -1;
    if (std::filesystem::is_directory(path)) return 1;
    if (!std::filesystem::is_regular_file(path)) return 1;

    // Attempt to launch
    const auto result = reinterpret_cast<intptr_t>(
        ShellExecuteA(nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT)
        );

    // Success
    if (result > 32) return 0;

    // Report Windows Error
    switch (result) {
        case ERROR_FILE_NOT_FOUND:  return -1;
        case ERROR_PATH_NOT_FOUND:  return -2;
        case ERROR_ACCESS_DENIED:   return -3;
        case SE_ERR_NOASSOC:        return -4;
        case SE_ERR_OOM:            return -5;
        default:                    return -6;
    }
}

std::vector<std::filesystem::directory_entry> FileHandler::list(const std::filesystem::path& path) {
    std::vector<std::filesystem::directory_entry> entries;
    if (!std::filesystem::is_directory(path)) return entries; //not a directory

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
