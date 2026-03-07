//header

#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <gtest/gtest.h>
#include "include/FileHandler.h"

const std::filesystem::path PROJECT_DIR = PROJECT_SOURCE_DIR;
const std::filesystem::path ENV = PROJECT_DIR / "env";

class FHandlerTest : public testing::Test {
protected:
    void SetUp() {

        std::cout << "FHandlerTest" << std::endl;
        std::cout << "ENV: " << ENV.generic_string() << std::endl;

        auto file = std::fstream(ENV / "file.txt", std::ios::out);

        ASSERT_TRUE(file.is_open());

        file << "Hello World!" << std::endl;
        file.close();

    }

    void TearDown() {

        for (const auto& entry : std::filesystem::directory_iterator(ENV)) {
            std::filesystem::remove_all(entry);
        }

    }

};


TEST_F(FHandlerTest, SetUpAndDestroy) {
    std::cout << "Completed SetUp; Starting TearDown" << std::endl;
}
