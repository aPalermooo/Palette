/******************************************
*   Name:           FileTagger.h
*   Description:    Header file for the tagging functions in the palette system
*   Author(s):      Hayden McVay <hm68s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/
#ifndef FILETAGGER_H
#define FILETAGGER_H

#include <string>
#include <vector>
#include <utility>

struct sqlite3;

class FileTagger {
private:
    std::string dbPath; // Path to the SQLite database file
    sqlite3* db;

    void initializeDatabase();
    int getOrCreateFileId(const std::string& filePath);
    int getOrCreateTagId(const std::string& tag);

public:
    explicit FileTagger(std::string path); // Init with path to SQLite file
    ~FileTagger();

    FileTagger(const FileTagger&) = delete;
    FileTagger& operator=(const FileTagger&) = delete;
    FileTagger(FileTagger&&) = delete;
    FileTagger& operator=(FileTagger&&) = delete;

    static void help(const std::string& command); // Help function to explain commands

    // Original tagging operations
    void addTag(const std::string& filePath, const std::string& tag);
    void removeTag(const std::string& filePath, const std::string& tag);

    void addFileToTag(const std::string& tag, const std::string& filePath);
    void removeFileFromTag(const std::string& tag, const std::string& filePath);

    std::vector<std::string> getTagsForFile(const std::string& filePath); // Returns a list of tags for a given file
    void verifyTagData(const std::string& tagFolder); // Verifies that all files in the tag data exist in the specified tag folder

    // Query methods for dynamic functionality
    /**
     * Retrieves all files associated with a specific tag
     * @param tag The tag name to search for
     * @return Vector of file paths that have the specified tag
     */
    std::vector<std::string> getFilesForTag(const std::string& tag);

    /**
     * Retrieves all tags in the system
     * @return Vector of all tag names
     */
    std::vector<std::string> getAllTags();

    // Bulk operations
    /**
     * Adds a tag to multiple files
     * @param filePaths Vector of file paths to tag
     * @param tag The tag to add to all files
     */
    void addTagToMultipleFiles(const std::vector<std::string>& filePaths, const std::string& tag);

    /**
     * Removes a tag from multiple files
     * @param filePaths Vector of file paths to remove tag from
     * @param tag The tag to remove from all files
     */
    void removeTagFromMultipleFiles(const std::vector<std::string>& filePaths, const std::string& tag);

    // Tag management operations
    /**
     * Deletes a tag entirely from the system
     * @param tag The tag name to delete
     */
    void deleteTag(const std::string& tag);

    /**
     * Renames an existing tag to a new name
     * @param oldTag The current tag name
     * @param newTag The new tag name
     */
    void renameTag(const std::string& oldTag, const std::string& newTag);

    /**
     * Checks if a tag exists in the system
     * @param tag The tag name to check
     * @return true if the tag exists, false otherwise
     */
    bool tagExists(const std::string& tag);

    /**
     * Gets the count of files associated with a tag
     * @param tag The tag name to query
     * @return Number of files that have the specified tag
     */
    int getFileCountForTag(const std::string& tag);
};
#endif //FileTagger_H