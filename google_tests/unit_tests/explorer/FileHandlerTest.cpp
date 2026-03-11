//header

#include <filesystem>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <gtest/gtest.h>

#include "ExplorerTest.h"
#include "explorer/FileHandler.h"

class FileHandlerTest : public HandlerTest {};

TEST_F(FileHandlerTest, SetUpAndDestroy) {
    std::cout << "Completed SetUp; Starting TearDown" << std::endl;
}
