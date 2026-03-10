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
#include <filesystem>
#include <vector>

class FileHandler {
private:

public:

    FileHandler() = default;

    static void openFile(const std::filesystem::path& path);

    static std::vector<std::filesystem::directory_entry> getDirectory(const std::filesystem::path& path);

};


#endif //PALETTE_FILEHANDLER_H