/******************************************
*   Name:           FileHandlerTest.cpp
*   Description:    Full coverage tests of File Handler Class Methods
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "ExplorerTest.h"
#include "explorer/FileHandler.h"

class FileHandlerTest : public HandlerTest {};

/**
 * @brief Tests creating a file (with no extension)
 * Covers: success, overwriters, and invalid names
 */
TEST_F(FileHandlerTest, createTest) {
    // Check Starting Conditions
    const std::string testFile = "testFile";
    const std::filesystem::path testFilePath = ENV / testFile;
    ASSERT_FALSE(std::filesystem::exists(testFilePath));

    // Create File - no overwrite -> Success (0)
    const auto handler = std::make_unique<FileHandler>();
    int result = handler->create(ENV, testFile);
    ASSERT_EQ(result, 0);
    ASSERT_TRUE(std::filesystem::exists(testFilePath));

    // Create File - attempted to overwrite  -> Fail (1)
    result = handler->create(ENV, testFile);
    ASSERT_EQ(result, 1);
    ASSERT_TRUE(std::filesystem::exists(testFilePath));

    // Create Invalid File -> Fail (-1)
    const std::string fakeFile = "A/B/A";
    const std::filesystem::path fakeFilePath = ENV / (fakeFile);
    ASSERT_FALSE(std::filesystem::exists(fakeFilePath));

    result = handler -> create(ENV, fakeFile);
    ASSERT_EQ(result, -1);
    ASSERT_FALSE(std::filesystem::exists(fakeFilePath));
}

/**
 * @brief Tests creating a file (with extension)
 * Covers: success, overwriters, and invalid names
 */
TEST_F(FileHandlerTest, createExtensionTest) {
    // Check Starting Conditions
    const std::string testFile = "testFile";
    const std::string testExtension = "txt";
    const std::filesystem::path testFilePath = ENV / (testFile + "." + testExtension);
    ASSERT_FALSE(std::filesystem::exists(testFilePath));


    // Create File - no overwrite -> Success (0)
    const auto handler = std::make_unique<FileHandler>();
    int result = handler -> create(ENV, testFile, testExtension);
    ASSERT_EQ(result, 0);
    ASSERT_TRUE(std::filesystem::exists(testFilePath));
    ASSERT_TRUE(std::filesystem::is_regular_file(testFilePath));

    // Create File - attempt to overwrite -> Fail (1)
    result = handler -> create(ENV, testFile, testExtension);
    ASSERT_EQ(result, 1);
    ASSERT_TRUE(std::filesystem::exists(testFilePath));
    ASSERT_TRUE(std::filesystem::is_regular_file(testFilePath));

    // Create Invalid File -> Fail (-1)
    const std::string fakeFile = "A/B/A";
    const std::filesystem::path fakeFilePath = ENV / (fakeFile + "." + testExtension);
    ASSERT_FALSE(std::filesystem::exists(fakeFilePath));

    result = handler -> create(ENV, fakeFile, testExtension);
    ASSERT_EQ(result, -1);
    ASSERT_FALSE(std::filesystem::exists(fakeFilePath));
}

/**
 * @brief Tests creating a file (with extension; No overwrite protection)
 * Covers: success, overwriters, and invalid names
 */
TEST_F(FileHandlerTest, forceCreateTest) {
    // Check Starting Conditions
    const std::string testFile = "testFile";
    const std::string testExtension = "txt";
    const std::filesystem::path testFilePath = ENV / (testFile + "." + testExtension);
    ASSERT_FALSE(std::filesystem::exists(testFilePath));

    // Create File - no overwrite -> Success (0)
    const auto handler = std::make_unique<FileHandler>();
    int result = handler -> forceCreate(ENV, testFile, testExtension);
    ASSERT_EQ(result, 0);
    ASSERT_TRUE(std::filesystem::exists(testFilePath));
    ASSERT_TRUE(std::filesystem::is_regular_file(testFilePath));

    // Create File - attempt to overwrite -> Success (0)
    result = handler -> forceCreate(ENV, testFile, testExtension);
    ASSERT_EQ(result, 0);
    ASSERT_TRUE(std::filesystem::exists(testFilePath));
    ASSERT_TRUE(std::filesystem::is_regular_file(testFilePath));

    // Create Invalid File -> Fail (-1)
    const std::string fakeFile = "A/B/A";
    const std::filesystem::path fakeFilePath = ENV / (fakeFile + "." + testExtension);
    ASSERT_FALSE(std::filesystem::exists(fakeFilePath));

    result = handler -> forceCreate(ENV, fakeFile, testExtension);
    ASSERT_EQ(result, -1);
    ASSERT_FALSE(std::filesystem::exists(fakeFilePath));
}

/**
 * @brief Tests removing files
 * Covers: Success, non-existent files, non-files,
 */
TEST_F(FileHandlerTest, removeTest) {
    // Check Starting Conditions
    const std::string targetFile = "Memo.txt";
    const std::filesystem::path targetFilePath = DOCUMENTS_DIR / targetFile;
    ASSERT_TRUE(std::filesystem::exists(targetFilePath));
    ASSERT_TRUE(std::filesystem::is_regular_file(targetFilePath));

    // Delete a file -> Success (0)
    const auto handler = std::make_unique<FileHandler>();
    int result = handler -> remove(targetFilePath);
    ASSERT_EQ(result, 0);
    ASSERT_FALSE(std::filesystem::exists(targetFilePath));

    // Delete a non-existent file -> Fail (1)
    result = handler -> remove(targetFilePath);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(std::filesystem::exists(targetFilePath));

    // Delete a directory -> Fail (1)
    result = handler -> remove(ENV);
    ASSERT_EQ(result, 1);
    ASSERT_TRUE(std::filesystem::exists(ENV));
}

/**
 * @brief Tests opening a file in its default application
 * Covers: Success, no available associations, non-existent files, directories
 */
TEST_F(FileHandlerTest, openTest) {
    // Open a regular file (txt) -> Success (0)
    //  Check Starting Conditions
    std::string targetFile = "Memo.txt";
    std::filesystem::path targetFilePath = DOCUMENTS_DIR / targetFile;
    ASSERT_TRUE(std::filesystem::exists(targetFilePath));
    ASSERT_TRUE(std::filesystem::is_regular_file(targetFilePath));

    //  Attempt to open
    const auto handler = std::make_unique<FileHandler>();
    int result = handler -> open(targetFilePath);
    ASSERT_EQ(result, 0);

    // Open a regular file (cpp) -> Success (0) || No association -> Fail (-4)
    //  Check Starting Conditions
    targetFile = "Malware.cpp";
    targetFilePath = DEVELOPMENT_DIR / targetFile;
    ASSERT_TRUE(std::filesystem::exists(targetFilePath));

    ASSERT_TRUE(std::filesystem::is_regular_file(targetFilePath));

    //  Attempt to open
    result = handler -> open(targetFilePath);
    ASSERT_TRUE(result == 0 || result == -4);

    // Open a directory -> Fail (1)
    result = handler -> open(ENV);
    ASSERT_EQ(result, 1);

    // Open a file that does not exist -> Fail (-1)
    const std::string fakeFile = "fake.txt";
    std::filesystem::path fakeFilePath = DOCUMENTS_DIR / fakeFile;
    result = handler -> open(fakeFilePath);
    ASSERT_EQ(result, -1);

    // Open a file in a directory that does not exist -> Fail (-2)
    const std::string fakeDirectory = "fakeDir";
    fakeFilePath = DOCUMENTS_DIR /  fakeDirectory /fakeFile;
    result = handler -> open(fakeFilePath);
    ASSERT_EQ(result, -2);
}

/**
 * @brief Tests listing paths of files in directories
 * Covers: Success, Empty Directories (with and without subdirectories), Non-existent Directories, Files
 */
TEST_F(FileHandlerTest, listTest) {
    // Check Starting conditions
    ASSERT_TRUE(std::filesystem::exists(ENV));

    // Check Empty Directory -> 0 entries
    const std::unique_ptr<FileHandler> handler = std::make_unique<FileHandler>();
    std::vector<std::filesystem::directory_entry> entries = handler -> list(PHOTO_DIR);
    ASSERT_EQ(entries.size(), 0);

    // Check Directory with no Files -> 0 entries
    entries = handler -> list(ENV);
    ASSERT_EQ(entries.size(), 0);

    // Check Directory with files -> correct count
    entries = handler -> list(DOCUMENTS_DIR);
    ASSERT_EQ(entries.size(), 5);

    // Check Non-existent Directory -> 0 entries
    const std::filesystem::path fakeDir = ENV / "fakeDir";
    entries = handler -> list(fakeDir);
    ASSERT_EQ(entries.size(), 0);

    // Check File -> 0 entries
    const std::filesystem::path fakeFile = ENV / "fakeFile";
    entries = handler -> list(fakeFile);
    ASSERT_EQ(entries.size(), 0);
}