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
#include "crow.h"


#include "TrayIcon.h"


/**
 * @brief Listener and Hooks
 */
void backgroundLoop () {

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() {
        return "Hello World";
    });

    app.port(18080).multithreaded().run();

}



int main() {

    std::cout << "ENVIRONMENT: " << std::endl;
    std::cout << "\tFile Structure Path: " << FILE_PATH << std::endl;
    std::cout << "\tDatabase Path: " << DB_PATH << std::endl;


    //Start Up
    TrayIcon trayIcon{HINSTANCE()};
    trayIcon.run();

    // Branch Background Process
    // Runtime Loop

    // Shutdown
    return 0;
}