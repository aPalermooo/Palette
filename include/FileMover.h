/***********************************
 *  Name:           FileMover.h
 *  Description:    Header file for reading from and creating directories
 *  Author(s):      Billy Caine <wc77s@missouristate.edu>
 *  Date:           March 2026
 *
 * Course: CSC450
 ***********************************/

#ifndef PALETTE_FILEMOVER_H
#define PALETTE_FILEMOVER_H

#include <filesystem>
#include <string>
#include "FileTagger.h"

class FileMover
{
private:
    std::filesystem::path rootPath;
    FileTagger& fileTagger;

public:
    FileMover(const std::filesystem::path& rootPath, FileTagger& fileTagger);
    void startUp();
    void refresh();

    void verifyRootPath();

    void verifyDirectories();

    void verifyFiles();
    void moveFile(const std::filesystem::path& filePath, const std::string& tag);

};


#endif //PALETTE_FILEMOVER_H