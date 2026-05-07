/***********************************
 *  Name:           Daemon.cpp
 *  Description:    Main Program
 *                  Background process that manages automatic systems (verification, tagging)
 *  Author(s):
 *  Date:           March 2026
 *
 * Course:          CSC450
 ***********************************/

#define _WIN32_WINNT 0x0A00

#include <iostream>
#include <ostream>
#include "API/Controller.h"
#include "PathVariables.h"

#include "TrayIcon.h"


static constexpr int PORT = 18080;

int main() {


    if (!std::filesystem::path(FILE_PATH).empty()) {
        PalettePath::filePath = std::filesystem::path(FILE_PATH);
        PalettePath::dbPath = std::filesystem::path(DB_PATH);
    }
    else {
        const char * home = std::getenv("USERPROFILE");
        const char * appdata = std::getenv("APPDATA");

        if (!home) {
            std::cerr << "Cannot find User Profile" << std::endl;
            return 1;
        }
        if (!appdata) {
            std::cerr << "Cannot find Appdata" << std::endl;
            return 1;
        }

        PalettePath::filePath = std::filesystem::path(home) / "Palette";
        PalettePath::dbPath = std::filesystem::path(appdata) / "Palette"/ "palette.db";

        if (!is_directory(PalettePath::filePath)) {
            std::filesystem::create_directory(PalettePath::filePath);
        }
        if (!is_directory(PalettePath::dbPath.parent_path())) {
            std::filesystem::create_directory(PalettePath::dbPath.parent_path());
        }
    }


    std::cout << "ENVIRONMENT: " << std::endl;
    std::cout << "\tFile Structure Path: " << PalettePath::filePath << std::endl;
    std::cout << "\tDatabase Path: " << PalettePath::dbPath << std::endl;


    //Start Up (Initialize components)
    TrayIcon trayIcon{HINSTANCE()};
    auto api = Controller(PORT);


    // Branch Background Process
    // Runtime Loop
    api.start();
    trayIcon.run();

    // Shutdown (Gather threads)
    api.stop();

    return 0;
}