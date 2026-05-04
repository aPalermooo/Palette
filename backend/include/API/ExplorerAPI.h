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


    // Get contents of a directory
    CROW_ROUTE(app, "/explorer/get-directory-contents") // ex: localhost:18080/explorer/get-directory-contents?path=<absolute_path>
    .methods("GET"_method)
    ([](const crow::request& request) {
        std::cout << "URL: " << request.url << std::endl;
        std::cout << "Query: " << request.raw_url << std::endl;
        std::cout << "Path param: " << request.url_params.get("path") << std::endl;
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

    // Open directory or file in default application
    CROW_ROUTE(app, "/explorer/open-default")
    .methods("GET"_method)
    ([](const crow::request& request) {
        try {

            // Validate input
            const char* pathRaw = request.url_params.get("path");

            if (!pathRaw) return crow::response(400); // No directory given in request

            const std::filesystem::path path(pathRaw);

            if (!std::filesystem::exists(path)) return crow::response(404);
            //

            // Process Input
            crow::json::wvalue body;

            const std::unique_ptr<Handler> handler = Handler::getHandler(path);

            const auto val = handler -> open(path);
            body["status"] = (val >= 0) ? "success" : "failure";
            body["code"] = val;


            switch(val){
                //Success Conditions
                case 0:
                    body["message"] = "Default Application opened successfully.";
                    return crow::response(body);
                    break;
                case 1:
                    body["message"] = "File explorer opened successfully.";
                    return crow::response(body);
                    break;

                // Failure Conditions
                case -1:
                case -2:
                    body["message"] = "Path could not be found.";
                    return crow::response(400, body);
                    break;
                case -3:
                    body["message"] = "Access denied.";
                    return crow::response(401, body);
                    break;
                case -4:
                    body["message"] = "No default application registered.";
                    break;
                case -5:
                    body["message"] = "System out of memory.";
                    break;
                case -6:
                default:
                    body["message"] = "Unknown error.";
                    break;
            } //end switch
            return crow::response(500, body); // Unhandled
        }//end try
        catch (std::filesystem::filesystem_error& e) { // Not valid path
            return crow::response(400);
        }//end catch
    });//end route
}

#endif //PALETTE_EXPLORER_H