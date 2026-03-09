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
#include <nlohmann/json.hpp> // Double check that library is incuded in the project

class FileTagger {
    private:
        std::string jsonPath; // Path to the JSON file that stores the tag data
        nlohmann::json tagData; // JSON object to hold the tag data

        void loadJson();
        void saveJson();

    public:
        FileTagger(const std::string& path); // Init with path to JSON file

        void addTag(const std::string& filePath, const std::string& tag);
        void removeTag(const std::string& filePath, const std::string& tag);
        
        void addFileToTag(const std::string& tag, const std::string& filePath);
        void removeFileFromTag(const std::string& tag, const std::string& filePath);

        std::vector<std::string> getTagsForFile(const std::string& filePath); // Returns a list of tags for a given file
        void verifyTagData(const std::string& tagFolder); // Verifies that all files in the tag data exist in the specified tag folder
};
#endif //FileTagger_H
