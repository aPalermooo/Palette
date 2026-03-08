/******************************************
*   Name:           FileHandler.h
*   Description:    Class responsible for servicing the front end and handing interactions with files
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#ifndef PALETTE_FILEHANDLER_H
#define PALETTE_FILEHANDLER_H
#include "src/gtest-internal-inl.h"

class FileHandler {
private:

public:

    FileHandler() = default;

    static void openFile(const std::filesystem::path& path);

    static std::vector<std::filesystem::directory_entry> getDirectory(const std::filesystem::path& path);

};

inline void FileHandler::openFile(const std::filesystem::path& path) {
    ShellExecuteA(nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
}

inline std::vector<std::filesystem::directory_entry> FileHandler::getDirectory(const std::filesystem::path& path) {
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        entries.push_back(entry);
    }
    return entries;
}


#endif //PALETTE_FILEHANDLER_H