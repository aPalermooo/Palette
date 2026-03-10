/*******************************
 *  Name:           FileMover.cpp
 *  Description:    Contains functions for creating directories and moving files as well as startup functions
 *  Author(s):      Billy Caine <wc77s@missouristate.edu>
 *  Date:           March 2026
 *
 *  Course:         CSC450
 ******************************/

#include "FileMover.h"
#include <iostream>
#include <vector>
#include <filesystem>


//Use filesystem namespace
namespace fs = std::filesystem;

//Constructor
FileMover::FileMover(const std::filesystem::path& rootPath, FileTagger& fileTagger) : rootPath(rootPath), fileTagger(fileTagger)
{
}

//Verify the root path directory exists, if not, create one (Only created on first startup ideally)
void FileMover::verifyRootPath()
{
    if (!fs::exists(rootPath))
    {
        fs::create_directory(rootPath);
    }
}

//Verifies directories exist, if not, call createDirectories to make them
void FileMover::verifyDirectories()
{
    fs::path documentsPath = rootPath / "documents";
    fs::path imagesPath = rootPath / "images";
    fs::path audioPath = rootPath / "audio";
    fs::path videoPath = rootPath / "video";
    fs::path otherPath = rootPath / "other";

    if (!exists(documentsPath))
    {
        fs::create_directory(documentsPath);
    }
    if (!exists(imagesPath))
    {
        fs::create_directory(imagesPath);
    }
    if (!exists(audioPath))
    {
        fs::create_directory(audioPath);
    }
    if (!exists(videoPath))
    {
        fs::create_directory(videoPath);
    }
    if (!exists(otherPath))
    {
        fs::create_directory(otherPath);
    }
}
//Verifies that file is an actual file and gets tag, handles no tag
void FileMover::verifyFiles()
{
    for (const auto& entry : fs::directory_iterator(rootPath))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
    }
}

