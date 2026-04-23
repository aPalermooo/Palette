/***********************************
 *  Name:           TaggerAPI.h
 *  Description:    Endpoints
 *                  Registers endpoints accessing FileTagger functionality
 *  Author(s):      Hayden McVay <hm68s@missouristate.edu>
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/

#ifndef PALETTE_TAGGERAPI_H
#define PALETTE_TAGGERAPI_H

#include "WhitelistMiddleware.h"
#include "crow/app.h"
#include "FileTagger.h"
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>

/**
 * Creates a standardized API error response for tagger endpoints.
 *
 * @param code HTTP status code to return
 * @param message Human-readable error message
 * @return JSON body in the form { "success": false, "error": <message> }
 */
inline crow::response makeTaggerError(const int code, const std::string& message)
{
    crow::json::wvalue body;
    body["success"] = false;
    body["error"] = message;
    return crow::response(code, body);
}

/**
 * Creates a standardized API success response for tagger endpoints.
 *
 * @param data Endpoint-specific payload
 * @return JSON body in the form { "success": true, "data": <data> }
 */
inline crow::response makeTaggerSuccess(crow::json::wvalue data) {
    crow::json::wvalue body;
    body["success"] = true;
    body["data"] = std::move(data);
    return crow::response(200, body);}

/**
 * Resolves the database path from compile definitions.
 *
 * @pre CMake defines DB_PATH compile definition
 * @return non-empty path to SQLite database
 * @throws std::runtime_error when DB_PATH is missing or empty
 */
inline std::string requireDBPath()
{
#ifdef DB_PATH
    const std::string dbPath = DB_PATH;
#else
    const std::string dbPath;
#endif

    if (dbPath.empty())
    {
        throw std::runtime_error("ERROR: Check CMAKE COMPILE DEF for DB_PATH");
    }

    return dbPath;
}

/**
 * Registers FileTagger API routes.
 *
 * Endpoints:
 * - GET  /tagger/get-tags?path=<absolute_path>
 * - POST /tagger/add-tag      { "path": "...", "tag": "..." }
 * - POST /tagger/remove-tag   { "path": "...", "tag": "..." }
 *
 * @param app Crow application instance
 */
inline void TaggerAPI (crow::App<WhitelistMiddleware>& app) {
    // register endpoints here...
    // GET TAGS
    CROW_ROUTE(app, "/tagger/get-tags")
    .methods("GET"_method)
    ([](const crow::request& request)
    {
        try
        {
            const char* pathRaw = request.url_params.get("path");
            if (!pathRaw || std::string(pathRaw).empty())
            {
                return makeTaggerError(400, "No file path provided in request");
            }

            const std::filesystem::path path(pathRaw);
            if (!std::filesystem::exists(path))
            {
                return makeTaggerError(404, "File does not exist");
            }

            FileTagger tagger(requireDBPath());
            const auto tags = tagger.getTagsForFile(path.string());

            crow::json::wvalue::list tagList;
            for (const auto& tag : tags)
            {
                tagList.emplace_back(tag);
            }

            crow::json::wvalue data;
            data["path"] = path.string();
            data["tags"] = std::move(tagList);
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });
    // ADD TAG
    CROW_ROUTE(app, "/tagger/add-tag")
    .methods("POST"_method)
    ([](const crow::request& request)
    {
        try
        {
            const auto body = crow::json::load(request.body);
            if (!body)
            {
                return makeTaggerError(400, "Invalid JSON in request body");
            }

            if (!body.has("path") || !body.has("tag"))
            {
                return makeTaggerError(400, "Missing 'path' or 'tag' in request body");
            }

            const std::string path = body["path"].s();
            const std::string tag = body["tag"].s();

            if (path.empty() || tag.empty())
            {
                return makeTaggerError(400, "Path / Tag cannot be empty");
            }

            if (!std::filesystem::exists(path))
            {
                return makeTaggerError(404, "File does not exist");
            }

            FileTagger tagger(requireDBPath());
            tagger.addTag(path, tag);

            crow::json::wvalue data;
            data["path"] = path;
            data["tag"] = tag;
            data["added"] = true;
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });
    // REMOVE TAG
    CROW_ROUTE(app, "/tagger/remove-tag")
    .methods("POST"_method)
    ([](const crow::request& request)
    {
        try
        {
            const auto body = crow::json::load(request.body);
            if (!body)
            {
                return makeTaggerError(400, "Invalid JSON body");
            }

            if (!body.has("path") || !body.has("tag"))
            {
                return makeTaggerError(400, "Missing 'path' or 'tag' in request body");
            }

            const std::string path = body["path"].s();
            const std::string tag = body["tag"].s();

            if (path.empty() || tag.empty())
            {
                return makeTaggerError(400, "Path / Tag cannot be empty");
            }

            FileTagger tagger(requireDBPath());
            tagger.removeTag(path, tag);

            crow::json::wvalue data;
            data["path"] = path;
            data["tag"] = tag;
            data["removed"] = true;
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });
}





#endif //PALETTE_TAGGERAPI_H