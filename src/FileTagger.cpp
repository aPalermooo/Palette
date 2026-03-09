/******************************************
*   Name:           FileTagger.cpp
*   Description:    Contains functions for tagging files in the palette system
*   Author(s):      Hayden McVay <hm68s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/
#include "FileTagger.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;

FileTagger::FileTagger(const std::string& path) : jsonPath(path) {
    loadJson();
}

/* LOAD JSON */
void FileTagger::loadJson() {
    std::ifstream inFile(jsonPath);

    if (inFile.is_open()) {
        inFile >> tagData;
        inFile.close();
    } else {
        // If the file doesn't exist, initialize an empty JSON object
        tagData = json::object();
    }
}

/* SAVE JSON */
void FileTagger::saveJson() {
    std::ofstream outFile(jsonPath);

    if (outFile.is_open()) {
        outFile << tagData.dump(4);
        outFile.close();
    }
}

/* ADD TAG TO FILE */
void FileTagger::addTag(const std::string& filePath, const std::string& tag) {

    // Ensure the file entry exists and is an array
    if (!tagData.contains(filePath)) {
        tagData[filePath] = json::array();
    }

    auto& tags = tagData[filePath];

    // Prevent duplicate tags
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
    }

    saveJson();
}

/* REMOVE TAG FROM FILE */
void FileTagger::removeTag(const std::string& filePath, const std::string& tag) {

    if (tagData.contains(filePath)) {
        auto& tags = tagData[filePath];

        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());

        saveJson();
    }
}

/* ADD FILE TO TAG */
void FileTagger::addFileToTag(const std::string& tag, const std::string& filePath) {

    // Ensure the file entry exists
    if (!tagData.contains(filePath)) {
        tagData[filePath] = json::array();
    }

    auto& tags = tagData[filePath];

    // Prevent duplicate tags
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
    }

    saveJson();
}

/* REMOVE FILE FROM TAG */
void FileTagger::removeFileFromTag(const std::string& tag, const std::string& filePath) {

    if (tagData.contains(filePath)) {
        auto& tags = tagData[filePath];

        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());

        saveJson();
    }
}

/* GET TAGS FOR FILE */
std::vector<std::string> FileTagger::getTagsForFile(const std::string& filePath) {

    if (tagData.contains(filePath)) {
        return tagData[filePath].get<std::vector<std::string>>();
    }

    return {};
}

/* VERIFY TAG DATA */
void FileTagger::verifyTagData(const std::string& tagFolder) {

    for (auto& [filePath, tags] : tagData.items()) {

        fs::path fullPath = fs::path(tagFolder) / filePath;

        if (!fs::exists(fullPath)) {
            std::cerr << "Warning: File " << fullPath << " does not exist." << std::endl;
        }
    }
}