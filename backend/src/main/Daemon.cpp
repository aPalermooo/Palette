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


#include "TrayIcon.h"


static constexpr int PORT = 18080;

int main() {

    std::cout << "ENVIRONMENT: " << std::endl;
    std::cout << "\tFile Structure Path: " << FILE_PATH << std::endl;
    std::cout << "\tDatabase Path: " << DB_PATH << std::endl;


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