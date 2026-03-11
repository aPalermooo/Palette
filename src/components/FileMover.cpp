/*******************************
 *  Name:           FileMover.cpp
 *  Description:    Contains functions for creating directories and moving files as well as startup functions
 *  Author(s):      Billy Caine <wc77s@missouristate.edu>
 *  Date:           March 2026
 *
 *  Course:         CSC450
 *******************************/

#include "FileMover.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>


//Use filesystem namespace
namespace fs = std::filesystem;

fs::path projectPath = PROJECT_SOURCE_DIR;
const auto tagger = std::make_unique<FileTagger>((projectPath / "src/resources/tags.json").string());

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
    for (const auto& entry : fs::recursive_directory_iterator(rootPath))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        else
        {
            std::string strPath = entry.path().string();
            std::vector<std::string> tagData = tagger -> getTagsForFile(strPath);
            if (tagData.empty())
            {
                std::string systemTag = createSystemTag(entry);
                tagger -> addTag(strPath, systemTag);

            }
        }


    }
}

//Creates system tags
std::string FileMover::createSystemTag(const fs::directory_entry& entry)
{

        std::string extension = entry.path().extension().string();
        if (extension == ".txt" || extension == ".rtf" || extension == ".docx" || extension == ".csv" ||
            extension ==".doc" || extension == ".wps" || extension == ".wpd"|| extension == ".msg")
        {
            return "Document";
        }
        else if (extension == ".jpg" || extension == ".png" || extension == ".webp" || extension == ".gif" ||
            extension == ".tif" || extension == ".bmp" || extension == ".eps")
        {
            return "Image";
        }
        else if (extension == ".mp3" || extension == ".wma" || extension == ".snd" || extension == ".wav" ||
            extension == ".ra" || extension == ".au" || extension == ".aac")
        {
            return "Audio";
        }
        else if (extension == ".mp4" || extension == ".3gp" || extension == ".avi" || extension == ".mpg" ||
            extension == ".wmv")
        {
            return "Video";
        }
        else
        {
            return "Other";
        }
}

//Moves file to correct directory
void FileMover::moveFile(const std::filesystem::path& filePath, const std::string& tag)
{
    fs::path destinationPath = rootPath / "tag";
    fs::path newLocation = destinationPath / filePath.filename();
    fs::rename(filePath, newLocation);

}

//Startup function
void FileMover::startUp()
{
    verifyRootPath();
    verifyDirectories();
    verifyFiles();
}

//Refresh function
void FileMover::refresh()
{
    verifyDirectories();
    verifyFiles();
}
