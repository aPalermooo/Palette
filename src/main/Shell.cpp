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
#include "explorer/DirectoryHandler.h"
#include "explorer/FileHandler.h"

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
    std::filesystem::path currentPath = applicationPath;

    const auto fileHandler = std::make_unique<FileHandler>();
    const auto directoryHandler = std::make_unique<DirectoryHandler>();

    while (true) {
        std::cout << "Palette-CLI " << currentPath.string() << ">  ";
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

            std::cout << "File Name: ";

            std::string file;
            getline(std::cin, file);

            const int dotPos = file.find('.');
            const std::string fileName = file.substr(0, dotPos);
            std::string extension;
            if (dotPos == std::string::npos)
                extension = "";
            else
                extension = file.substr(dotPos + 1);
            if (fileName.empty()) {
                std::cerr << "Invalid file name." << std::endl;
                continue;
            }

            int result = fileHandler -> create(currentPath, fileName, extension);

            if ( result == 1 ) {
                std::cout << "File " << fileName << " already exists. Overwrite? [y/n] ";
                std::string response;
                std::getline(std::cin, response);
                if (response == "y")
                    result = fileHandler -> forceCreate(currentPath, fileName, extension);
                else if (response == "n")
                    continue;
            }

            if ( result == -1 ) {
                std::cout << "File could not be created" <<std::endl;
                continue;
            }

            std::cout << "File created." << std::endl;
        }
        if (command == "delete") {
            std::cout << "selection: ";
            std::string selection;
            std::getline(std::cin, selection);

            const auto selectedPath = currentPath / selection;
            const auto handler = Handler::getHandler(selectedPath);

            int result = handler -> remove(selectedPath);
            if (result == 1)
                std::cout << "Invalid Selection" << std::endl;
            else if (result == -1 && dynamic_cast<DirectoryHandler*>(handler.get()) != nullptr) {

            }
        }
        if (command == "open") {
            // Nav to child director/Open file in default application
            std::cout << "selection: ";
            std::string selection;
            std::getline(std::cin, selection);

            const auto selectedPath = currentPath / selection;
            const auto handler = Handler::getHandler(selectedPath);

            const int result = handler -> open(selectedPath);
        }
        if (command == "forward") {
            std::cout << "selection: ";
            std::string selection;
            std::getline(std::cin, selection);

            const auto subDir = currentPath / selection;
            if (std::filesystem::is_directory(subDir))
                currentPath = subDir;
            else
                std::cout << "Tag does not exist." << std::endl;
        }
        if (command == "back") {
            // Return to parent directory
            if (currentPath == applicationPath)     continue;
            currentPath = currentPath.parent_path();
        }
        if (command == "list") {
            const auto directory_iterator = directoryHandler -> list(currentPath);
            const auto file_iterator = fileHandler -> list(currentPath);

            if (!directory_iterator.empty()) {          // If Sub tag exist
                std::cout << "Tags:" << std::endl;
                for (const auto& directory : directory_iterator) {
                    std::cout << "\t" << directory.path().filename().string() << std::endl;
                }
            }

            std::cout << "Files:" << std::endl;
            if (!file_iterator.empty()) {
                for (const auto& file : file_iterator) {
                    std::cout << "\t" << file.path().filename().string() << std::endl;
                }
            }
            else
                std::cout << "\tEmpty" << std::endl;
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
