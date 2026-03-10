/******************************************
*   Name:           Shell.cpp
*   Description:    Runs CLI interface of palette
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu> <Primary>
*                   Hayden McVay <hm68s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>
#include "FileTagger.h"

// This file will contain the implementation of the CLI environment
// It will handle taking commands from the user, and executing the corresponding


int main(int argc, char* argv[]) {

    //TODO: Break up into separate functions

    // Locate the directory that Palette stores files in
    std::filesystem::path applicationPath;
    if (argc >= 2) { // Debug mode; run in env (dynamic testing)
        if (std::string(argv[1]) == "-d" ) {
            applicationPath = std::filesystem::path(PROJECT_SOURCE_DIR) / "env";
        }
    }
    else {
        const char * home = std::getenv("USERPROFILE");

        if (!home) {
            std::cerr << "Cannot find User Profile" << std::endl;
            return 1;
        }

        applicationPath = std::filesystem::path(home) / "Palette";
    }

    // TODO:
    // Start up Functionality


    // Start CLI

    FileTagger tagger((applicationPath / "tags.json").string());
    std::string command;

    while (true) {
        std::cout << "Palette-CLI " << applicationPath.string() << ">  ";
        std::getline(std::cin, command);

        //parse command

        if (command == "tag") {
            //Open tagging menu
            std::cout << "Tagging Logic" << std::endl;

            // Adding Tagging Logic - Hayden
            // READ ME - file input takes string because I have yet
            // to figure out how to do the file sys shit.
            std::string action;
            std::string file;
            std::string tag;

            std::cout << "Enter command (add/remove/list): "; // Add tag help cmd
            std::cin >> action;

            if (action == "add") {

                std::cout << "File: ";
                std::cin >> file;

                std::cout << "Tag: ";
                std::cin >> tag;

                tagger.addTag(file, tag);

                std::cout << "Tag added." << std::endl;
            }
            if (action == "remove") {
                std::cout << "File: ";
                std::cin >> file;

                std::cout << "Tag: ";
                std::cin >> tag;

                tagger.removeTag(file, tag);

                std::cout << "Tag removed." << std::endl;
            }
            if (action == "list") {
                std::cout << "File: ";
                std::cin >> file;

                auto tags = tagger.getTagsForFile(file);

                std::cout << "Tags: " << file << ": ";
                for (const auto& t : tags) {
                    std::cout << t << " ";
                }
                std::cout << std::endl;
            }// Potential change to else if for default error handling

            std::cin.ignore(); // Clear newline from input buffer
        }
        if (command == "new") {
            // Create new file, proceed to tag it/refresh
            std::cout << "New File Logic" << std::endl;
        }
        if (command == "open") {
            // Nav to child director/Open file in default application
            std::cout << "Opening Logic" << std::endl;
        }
        if (command == "back") {
            // Return to parent directory
            std::cout << "Retrace Logic" << std::endl;
        }
        if (command == "refresh") {
            // Call Start-Up Function again
            std::cout << "Refreshing Logic" << std::endl;
        }
        if (command == "exit") {
            // Exit Program
            exit(0);
        }
    }
}
