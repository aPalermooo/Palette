//
// Created by on 3/9/2026.
//

#include "FileHandler.h"
#include <windows.h>

void FileHandler::openFile(const std::filesystem::path& path) {
    if (!std::filesystem::is_directory(path))
        ShellExecuteA(nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
}

std::vector<std::filesystem::directory_entry> FileHandler::getDirectory(const std::filesystem::path& path) {
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(path))
        entries.push_back(entry);
    return entries;
}
