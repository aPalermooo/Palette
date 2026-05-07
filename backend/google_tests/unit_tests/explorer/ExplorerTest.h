/******************************************
*   Name:           ExplorerTest.h
*   Description:    Creates test ENV folder to complete full coverage tests of module
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#ifndef PALETTE_EXPLORERTEST_H
#define PALETTE_EXPLORERTEST_H
#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

/**
 * @brief Fills a directory with filler files to test functionality
 *
 * @param directory Absolute path of the directory being targeted
 * @param fileNames Name of each file being created in the directory
 * @param extension File type of each file being created
 */
inline void populate (const std::filesystem::path &directory, const std::vector<std::string> &fileNames, const std::string &extension) {
    if (!std::filesystem::is_directory(directory)) return;
    std::fstream writer;
    std::string file;
    for (const auto& fileName : fileNames) {
        file = fileName + extension;
        writer = std::fstream(directory / file, std::ios::out);
        writer << "Hello World!" << std::endl;
        writer.close();
    }
}

class HandlerTest : public testing::Test {
protected:

    // Locate Project source directory
    const std::filesystem::path PROJECT_DIR = PROJECT_SOURCE_DIR;
    const std::filesystem::path ENV = PROJECT_DIR / "env";      //simulated palette folder

    // Mock directories
    const std::filesystem::path DOCUMENTS_DIR = ENV / "documents";
    const std::filesystem::path PHOTO_DIR = ENV / "photo";
    const std::filesystem::path DEVELOPMENT_DIR = ENV / "development";


    /**
     * Create a mock environment of various directories and files
     */
    void SetUp() override {

        std::cout << "ENV: " << ENV.generic_string() << std::endl;

        ASSERT_TRUE(std::filesystem::exists(ENV));
        ASSERT_TRUE(std::filesystem::is_directory(ENV));

        // Create mock directories
        std::filesystem::create_directory(DOCUMENTS_DIR);
        std::filesystem::create_directory(PHOTO_DIR);           //Empty Directory
        std::filesystem::create_directory(DEVELOPMENT_DIR);


        //Create files in directories
        const std::vector<std::string> docNames = {"file", "2026-Taxes", "Resolutions", "Memo", "Resume"};
        const std::string docExtension = ".txt";

        const std::vector<std::string> devNames = {"Malware", "Virus", "Worm"};
        const std::string devExtension = ".cpp";

        populate(DOCUMENTS_DIR, docNames, docExtension);
        populate(DEVELOPMENT_DIR, devNames, devExtension);
    }

    /**
     * Remove all contents in environment to reset
     */
    void TearDown() override {
        for (const auto& entry : std::filesystem::directory_iterator(ENV)) {
            std::filesystem::remove_all(entry);
        }
    }
};

#endif //PALETTE_EXPLORERTEST_H