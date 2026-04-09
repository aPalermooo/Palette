/***********************************
 *  Name:           Daemon.cpp
 *  Description:    Main Program
 *                  Background process that manages automatic systems (verification, tagging)
 *  Author(s):
 *  Date:           March 2026
 *
 * Course:          CSC450
 ***********************************/


#include <iostream>
#include <ostream>
#include "crow.h"

#include "database/Database.h"

static std::filesystem::path dbPath;
static std::filesystem::path filePath;
static std::unique_ptr<Database> db;

/**
 * @brief Runs at the start of application; prepares environment for runtime
 *
 * @note In case of runtime failure, call StartUp to refresh
 */
void StartUp () {

    // Find File Structure and Database Locations
    filePath = FILE_PATH;
    if (filePath.empty()) {
        // Locate/Set to proper location
    }
    dbPath = DB_PATH;
    if (dbPath.empty()) {
        // Locate/Set to proper location
    }

    // Establish Database connection
    db = std::make_unique<Database>(dbPath);
}

/**
 * @brief Listener and Hooks
 */
void BackgroundLoop ();

/**
 * @brief Safely exit program; safe all data
 */
void ShutDown () {
    db.reset();
}


int main() {

    std::cout << "ENVIRONMENT: " << std::endl;
    std::cout << "\tFile Structure Path: " << FILE_PATH << std::endl;
    std::cout << "\tDatabase Path: " << DB_PATH << std::endl;

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() {
        return "Hello World";
    });

    app.port(18080).multithreaded().run();

    // Start Up

    // Branch Background Process
    // Runtime Loop

    // Shutdown
    return 0;
}