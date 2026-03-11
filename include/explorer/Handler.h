/******************************************
*   Name:           DirectoryHandler.h
*   Description:    Declaration - Abstract class for navigating files and directories
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu> <Primary>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#ifndef PALETTE_HANDLER_H
#define PALETTE_HANDLER_H

#include <filesystem>
#include <vector>

// temp definition
class FileHandler;
class DirectoryHandler;

/**
 * @class Handler
 * @brief Abstract class interface describing the functionality of the explorer helper package
 */
class Handler {
    public:

    //Default constructor and destructor
    Handler() = default;

    virtual ~Handler() = default;

    /**
     * @brief Create an item at the given path
     *
     * @param path the absolute path of the directory the item will be placed in
     * @param name the name of the item to be created
     * @return 0 on success, non-zero on failure
     */
    virtual int create(const std::filesystem::path& path, const std::string& name) = 0;

    /**
     * @brief Remove an item at the given path
     *
     * @param path the absolute path of the item to be deleted
     * @return 0 on success, non-zero on failure
     */
    virtual int remove(const std::filesystem::path& path) = 0;

    /**
     * Opens item in its default application
     * @param path Absolute path to the item to be opened
     * @return void
     */
    virtual void open(const std::filesystem::path& path) = 0;

    /**
     * @brief Returns a list of a given item in a directory
     *
     * @param path the absolute path to the directory being scanned
     * @return a dynamic list of items contained in a directory
     */
    virtual std::vector<std::filesystem::directory_entry> list(const std::filesystem::path &path) = 0;

    /**
     * @brief Identifies the handler that is required for the path that is provided
     * @param path Path to the item to be identified
     * @return Handler object that is associated with the item provided
     */
    static std::unique_ptr<Handler> getHandler(const std::filesystem::path& path);
};

#endif //PALETTE_HANDLER_H