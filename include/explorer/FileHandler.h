/******************************************
*   Name:           FileHandler.h
*   Description:    Declaration - Class responsible for servicing the front end and handing interactions with files
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#ifndef PALETTE_FILEHANDLER_H
#define PALETTE_FILEHANDLER_H

#include <filesystem>
#include <vector>

#include <explorer/Handler.h>

class FileHandler : public Handler{
public:

    /**
     * @brief Create a file (with no extension) at the given path, if it does not already exist.
     *
     * Will never overwrite an existing file
     *
     * @pre File does not yet exist
     * @post Empty file exists
     *
     * @param current the absolute path of the directory containing the item
     * @param name the name of the item to be created
     * @return 0 on success
     * @return 1 File already exists
     * @return -1 File could not be created
     */
    int create(const std::filesystem::path& current, const std::string& name) override;

    /**
     * @brief Create a file at the given path, if it does not already exist.
     *
     * Will never overwrite an existing file
     *
     * @pre File does not yet exist
     * @post Empty file exists
     *
     * @param current the absolute path of the directory containing the item
     * @param name the name of the item to be created
     * @param extension the type of file being created
     * @return 0 on success
     * @return 1 File already exists
     * @return -1 File could not be created
     */
    int create(const std::filesystem::path& current, const std::string& name, const std::string& extension);

    /**
     * @brief Remove an item at the given path
     *
     * @param path the absolute path of the item to be deleted
     * @return 0 on success
     * @return 1 File already does not exist
     * @return -1 File could not be deleted
     */
    int remove(const std::filesystem::path& path) override;

    /**
     * Opens a File in default application set by Windows settings
     *
     * @pre file exists
     *
     * @param path Absolute path to the file to be opened
     */
    void open(const std::filesystem::path& path) override;

    /**
     * @brief Returns a list of files in a directory
     *
     * @pre parent directory exists
     *
     * @param path the absolute path to the directory being scanned
     * @return a dynamic list of subdirectory paths contained in a directory
     */
    std::vector<std::filesystem::directory_entry> list(const std::filesystem::path &path) override;

    /**
     * @brief Create a file at the given path.
     *
     * WILL OVERWRITE EXISTING FILES
     *
     * @pre File does not yet exist
     * @post Empty file exists
     *
     * @param current the absolute path of the directory containing the item
     * @param name the name of the item to be created
     * @param extension the type of file being created
     * @return 0 on success
     * @return 1 File already exists
     */
    int forceCreate(const std::filesystem::path& current, const std::string& name, const std::string& extension);
};

#endif //PALETTE_FILEHANDLER_H