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

    void addTag(const std::string& filePath, const std::string& tag);
    void removeTag(const std::string& filePath, const std::string& tag);

    void addFileToTag(const std::string& tag, const std::string& filePath);
    void removeFileFromTag(const std::string& tag, const std::string& filePath);

    std::vector<std::string> getTagsForFile(const std::string& filePath); // Returns a list of tags for a given file
    void verifyTagData(const std::string& tagFolder); // Verifies that all files in the tag data exist in the specified tag folder
};
#endif //FileTagger_H