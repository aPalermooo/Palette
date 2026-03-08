/******************************************
*   Name:           FileHandler.h
*   Description:    Singleton class responsible for servicing the front end and handing interactions with files
*   Author:         Xander Palermo <ajp2s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#ifndef PALETTE_FILEHANDLER_H
#define PALETTE_FILEHANDLER_H

class FileHandler {
private:

    static FileHandler* instancePtr;

    static std::mutex mtx;

    FileHandler();
public:

    FileHandler(const FileHandler& obj) = delete; // prevent copies

    static FileHandler* getInstance();

};

FileHandler* FileHandler::instancePtr = nullptr;
std::mutex FileHandler::mtx;

inline FileHandler* FileHandler::getInstance() {
    if (instancePtr == nullptr) {
        std::lock_guard<std::mutex> lock(mtx);
        if (instancePtr == nullptr) {
            instancePtr = new FileHandler();
        }
    }
    return instancePtr;
}

#endif //PALETTE_FILEHANDLER_H