/***********************************
 *  Name:           SearchAPI.h
 *  Description:    File search endpoints
 *  Author(s):      Hayden McVay <hm68s@missouristate.edu>
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/

#ifndef PALETTE_SEARCHAPI_H
#define PALETTE_SEARCHAPI_H

#include "WhitelistMiddleware.h"
#include "crow/app.h"
#include "FileSearcher.h"
#include <stdexcept>
#include <string>
#include <vector>

#include "PathVariables.h"

/**
 * Creates a standardized API response.
 *
 * Formats API responses consistently across all endpoints with a success flag,
 * optional error message, and response data payload. Returns HTTP 200 for success,
 * HTTP 400 for errors.
 *
 * @param success Operation status (true for success, false for error)
 * @param message Error message (only used when success is false)
 * @param data Response payload containing the actual data (only used when success is true)
 * @return JSON response body with HTTP status code
 */
inline crow::response makeSearchResponse(bool success, const std::string& message, crow::json::wvalue data = crow::json::wvalue())
{
    crow::json::wvalue body;
    body["success"] = success;
    if (!success) {
        body["error"] = message;
    } else {
        body["data"] = std::move(data);
    }
    return crow::response(success ? 200 : 400, body);
}

/**
 * Resolves database path from CMake compile definitions.
 *
 * Retrieves the database path set during CMake configuration. This path is required
 * for all database operations. Throws an exception if the database path is not properly
 * configured during the build process.
 *
 * @throws std::runtime_error if DB_PATH is not defined or empty
 * @return Path to SQLite database as a string
 */
inline std::string requireSearchDBPath()
{
    return PalettePath::dbPath.string();
}

/**
 * Registers file search API endpoints.
 *
 * Provides three main search endpoints for querying files by tag, filename pattern,
 * or a combination of both. Supports filtering, multi-tag searches with AND/OR logic,
 * and result sorting.
 *
 * Endpoints:
 * - GET  /search/tag?name=<tag_name>
 *   Search for files by a single tag name
 *   Returns: { "success": bool, "data": { "tag": string, "count": int, "files": [...] } }
 *
 * - GET  /search/filename?pattern=<pattern>
 *   Search for files matching a filename pattern (SQL LIKE syntax with %)
 *   Returns: { "success": bool, "data": { "pattern": string, "count": int, "files": [...] } }
 *
 * - POST /search
 *   Unified search endpoint supporting multiple criteria:
 *   Request body:
 *     {
 *       "tags": ["tag1", "tag2"],           (optional) Array of tags to search
 *       "filename": "%pattern%",             (optional) Filename pattern to match
 *       "requireAllTags": true/false,        (optional, default: true) AND vs OR logic for tags
 *       "sort": "asc"/"desc"                 (optional, default: "asc") Sort results by filename
 *     }
 *   Returns: { "success": bool, "data": { "count": int, "files": [...] } }
 *
 * @param app Crow application instance to register routes with
 */
inline void SearchAPI(crow::App<WhitelistMiddleware>& app) {

    // SEARCH BY TAG
    CROW_ROUTE(app, "/search/tag")
    .methods("GET"_method)
    ([](const crow::request& req) {
        try {
            const char* tagRaw = req.url_params.get("name");
            if (!tagRaw || std::string(tagRaw).empty()) {
                return makeSearchResponse(false, "Tag name required");
            }

            FileSearcher searcher(requireSearchDBPath());
            auto results = searcher.searchByTag(tagRaw);

            crow::json::wvalue::list fileList;
            for (const auto& file : results) {
                fileList.emplace_back(file);
            }

            crow::json::wvalue data;
            data["tag"] = tagRaw;
            data["count"] = static_cast<int>(results.size());
            data["files"] = std::move(fileList);
            return makeSearchResponse(true, "", std::move(data));
        } catch (const std::exception& e) {
            return makeSearchResponse(false, e.what());
        }
    });

    // SEARCH BY FILENAME
    CROW_ROUTE(app, "/search/filename")
    .methods("GET"_method)
    ([](const crow::request& req) {
        try {
            const char* patternRaw = req.url_params.get("pattern");
            if (!patternRaw || std::string(patternRaw).empty()) {
                return makeSearchResponse(false, "Pattern required");
            }

            FileSearcher searcher(requireSearchDBPath());
            auto results = searcher.searchByFileNamePattern(patternRaw);

            crow::json::wvalue::list fileList;
            for (const auto& file : results) {
                fileList.emplace_back(file);
            }

            crow::json::wvalue data;
            data["pattern"] = patternRaw;
            data["count"] = static_cast<int>(results.size());
            data["files"] = std::move(fileList);
            return makeSearchResponse(true, "", std::move(data));
        } catch (const std::exception& e) {
            return makeSearchResponse(false, e.what());
        }
    });

    // UNIFIED SEARCH (tags + filename + sorting)
    CROW_ROUTE(app, "/search")
    .methods("POST"_method)
    ([](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);
            if (!body) {
                return makeSearchResponse(false, "Invalid JSON");
            }

            std::vector<std::string> tags;
            std::string namePattern;
            bool requireAllTags = true;
            bool sortAscending = true;

            if (body.has("tags")) {
                for (const auto& tag : body["tags"]) {
                    tags.push_back(tag.s());
                }
            }

            if (body.has("filename")) {
                namePattern = body["filename"].s();
            }

            if (body.has("requireAllTags")) {
                requireAllTags = body["requireAllTags"].b();
            }

            if (body.has("sort")) {
                sortAscending = body["sort"].s() == "asc";
            }

            FileSearcher searcher(requireSearchDBPath());
            std::vector<std::string> results;

            if (!namePattern.empty() && !tags.empty()) {
                results = searcher.searchByNameAndTags(namePattern, tags, requireAllTags);
            } else if (!tags.empty()) {
                results = searcher.searchByMultipleTags(tags, requireAllTags);
                if (sortAscending) {
                    results = searcher.searchByTagsThenName(tags, sortAscending);
                }
            } else if (!namePattern.empty()) {
                results = searcher.searchByFileNamePattern(namePattern);
            } else {
                return makeSearchResponse(false, "Provide 'tags' or 'filename'");
            }

            crow::json::wvalue::list fileList;
            for (const auto& file : results) {
                fileList.emplace_back(file);
            }

            crow::json::wvalue data;
            data["count"] = static_cast<int>(results.size());
            data["files"] = std::move(fileList);
            return makeSearchResponse(true, "", std::move(data));
        } catch (const std::exception& e) {
            return makeSearchResponse(false, e.what());
        }
    });
}

#endif //PALETTE_SEARCHAPI_H
