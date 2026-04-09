/******************************************
*   Name:           AbstractHandlerTest.cpp
*   Description:    Full coverage tests of the Abstract Handler Class Method
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#include "ExplorerTest.h"
#include "explorer/DirectoryHandler.h"
#include "explorer/FileHandler.h"
#include "explorer/Handler.h"
#include "gtest/gtest.h"

class AbstractHandlerTest : public HandlerTest {};

TEST_F(AbstractHandlerTest, getDirectoryHandler) {
    // Check File Structure
    ASSERT_TRUE(std::filesystem::exists(DOCUMENTS_DIR));
    ASSERT_TRUE(std::filesystem::is_directory(DOCUMENTS_DIR));

    // Create Object and check type
    const std::unique_ptr<Handler> testHandler = Handler::getHandler(DOCUMENTS_DIR);
    // ASSERT_NE(testHandler, nullptr);
    ASSERT_NE(dynamic_cast<DirectoryHandler*>(testHandler.get()), nullptr);
}

TEST_F (AbstractHandlerTest, getFileHandler) {
    // Check File Structure
    const std::filesystem::path targetFile = DOCUMENTS_DIR / "Memo.txt";
    ASSERT_TRUE(std::filesystem::exists(targetFile));
    ASSERT_FALSE(std::filesystem::is_directory(targetFile));
    ASSERT_TRUE(std::filesystem::is_regular_file(targetFile));

    // Create Object and check type
    const std::unique_ptr<Handler> testHandler = Handler::getHandler(targetFile);
    ASSERT_NE(testHandler, nullptr);
    ASSERT_NE(dynamic_cast<FileHandler*>(testHandler.get()), nullptr);
}
