/******************************************
*   Name:           DirectoryHandler.h
*   Description:    Declaration - Class responsible for servicing the front end and handing interactions with Directories
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu> <Primary>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/
#ifndef PALETTE_DIRECTORYNAV_H
#define PALETTE_DIRECTORYNAV_H

#include <vector>
#include <filesystem>

#include "Handler.h"

class DirectoryHandler : public Handler {
public:

    /**
     * @brief Create a new subdirectory at the given path
     *
     * @pre directory does not yet exist
     * @post directory is created with given name
     *
     * @param current the absolute path of the directory the subdirectory will be placed in
     * @param name the name of the directory to be created
     * @return 0 on success
     * @return 1 Directory already exists
     * @returns -1 Directory could not be created
     */
    int create(const std::filesystem::path& current, const std::string& name) override;

    /**
     * @brief Remove a subdirectory at the given path
     *
     * @pre directory is empty
     * @post directory is deleted
     *
     * @param path the absolute path of the subdirectory to be deleted
     * @return 0 on success
     * @return 1 Directory does not exist
     * @return -1 Directory could not be removed (may have contents)
     */
    int remove(const std::filesystem::path& path) override;

    /**
     * Opens a directory in Windows Explorer
     *
     * @pre directory exists
     *
     * @param path Absolute path to the directory to be opened
     */
    void open(const std::filesystem::path& path) override;

    /**
     * @brief Returns a list of subdirectories in a directory
     *
     * @pre parent directory exists
     *
     * @param path the absolute path to the directory being scanned
     * @return a dynamic list of subdirectory paths contained in a directory
     */
    std::vector<std::filesystem::directory_entry> list(const std::filesystem::path& path) override;

    /**
     * @brief Remove a subdirectory at the given path; All contents are also recursively deleted
     *
     * @pre directory exists
     * @post directory (and all of its children) are deleted
     *
     * @param path the absolute path of the subdirectory to be deleted
     * @return number of files/directories deleted
     * @return -1 directory does not exist
     * @return -2 directory could not be deleted
     */
    int forceRemove(const std::filesystem::path& path);

    /**
     * @brief Descends directory tree to next subdirectory
     *
     * @pre subdirectory exists
     *
     * @param current Absolute path of the current directory
     * @param target Subdirectory to be added to path
     * @return Absolute path of subdirectory
     * @return Current path if target does not exist
     */
    std::filesystem::path descend(const std::filesystem::path& current, const std::string& target);

    /**
     * @brief Ascends directory tree to parent directory
     *
     * @pre current directory is not root directory
     * @pre current directory is within set Palette Folder
     *
     * @param current Current directory path
     * @return Absolute path of parent directory
     * @return Current path if current directory is root
     */
    std::filesystem::path ascend(const std::filesystem::path& current);
};

#endif //PALETTE_DIRECTORYNAV_H
