/***********************************
 *  Name:           ExplorerAPI.h
 *  Description:    Endpoints
 *                  Registers endpoints accessing explorer functionality
 *  Author(s):      Xander Palermo <ajp2s@missouristate.edu
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/


#ifndef PALETTE_EXPLORER_H
#define PALETTE_EXPLORER_H
#include "WhitelistMiddleware.h"
#include "crow/app.h"
#include "explorer/DirectoryHandler.h"
#include "explorer/FileHandler.h"

/**
 *
 * Scans a given directory for desired items specified by handler object given
 *
 * DirectoryHandler     -> Returns list of subdirectories under current path
 * FileHandler          -> Returns list of files under current path
 *
 * @param handler Handler object used to locate paths in directory
 * @param path Directory being scanned
 * @pre path exists and is a directory
 * @return a list of JSON objects containing names and paths of children nodes of path
 * @return for paths that do not exist -> return an empty list
 */
inline std::vector<crow::json::wvalue> parseDirectoryItems(const std::unique_ptr<Handler>& handler, std::filesystem::path path) {
    std::vector<crow::json::wvalue> list;

    if (!std::filesystem::is_directory(path)) return list; // Path does not exist

    for (const auto& entry : handler->list(path)) {
        crow::json::wvalue item;
        item["name"] = entry.path().filename().string();
        item["path"] = entry.path().string();
        list.push_back(std::move(item));
    }
    return list;
}

inline void ExplorerAPI (crow::App<WhitelistMiddleware>& app) {

    CROW_ROUTE(app, "/explorer/get-directory-contents") // ex: localhost:18080/explorer/get-directory-contents?path=<absolute_path>
    .methods("GET"_method)
    ([](const crow::request& request) {
        try {
            // Validate input
            const char* pathRaw = request.url_params.get("path");

            if (!pathRaw) return crow::response(400);  // No directory given in request

            const std::filesystem::path path(pathRaw);

            if (!std::filesystem::is_directory(path)) return crow::response(404);  // Directory does not exist
           //

            //Process Input
            crow::json::wvalue body;

            const std::unique_ptr<Handler> fileHandler = std::make_unique<FileHandler>();
            const std::unique_ptr<Handler> dirHandler = std::make_unique<DirectoryHandler>();

            body["files"] = parseDirectoryItems(fileHandler, path);
            body["directories"] = parseDirectoryItems(dirHandler, path);

            return crow::response(body); //success
            //
        }
        catch (std::filesystem::filesystem_error& e) { // Not a valid path
            return crow::response(400);
        }
    });
}

#endif //PALETTE_EXPLORER_H