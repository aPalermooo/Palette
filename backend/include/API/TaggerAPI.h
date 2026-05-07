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
#include "DynamicTagger.h"
#include "TaggingRulesSetup.h"
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

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
    return crow::response(200, body);
}

/**
 * Resolves the database path from compile definitions.
 *
 * @pre CMake defines DB_PATH compile definition
 * @return non-empty path to SQLite database
 * @throws std::runtime_error when DB_PATH is missing or empty
 */
inline std::string requireDBPath()
{
    return PalettePath::dbPath.string();
}

/**
 * Global DynamicTagger instance for use across endpoints
 * Initialize this in your main application setup
 */
static std::unique_ptr<DynamicTagger> g_dynamicTagger = nullptr;

/**
 * Registers FileTagger API routes.
 *
 * Endpoints:
 * - GET  /tagger/get-tags?path=<absolute_path>
 * - POST /tagger/add-tag      { "path": "...", "tag": "..." }
 * - POST /tagger/remove-tag   { "path": "...", "tag": "..." }
 * - GET  /tagger/files-by-tag?tag=<tag_name>
 * - GET  /tagger/all-tags
 * - POST /tagger/apply-rules  { "path": "..." }
 * - POST /tagger/apply-rules-batch { "paths": [...] }
 * - GET  /tagger/rules
 * - POST /tagger/add-tag-multiple { "paths": [...], "tag": "..." }
 * - POST /tagger/remove-tag-multiple { "paths": [...], "tag": "..." }
 * - POST /tagger/delete-tag { "tag": "..." }
 * - POST /tagger/rename-tag { "oldTag": "...", "newTag": "..." }
 * - GET  /tagger/tag-exists?tag=<tag_name>
 * - GET  /tagger/tag-file-count?tag=<tag_name>
 *
 * @param app Crow application instance
 */
inline void TaggerAPI (crow::App<WhitelistMiddleware>& app) {

    // ==================== ORIGINAL ENDPOINTS ====================

    // GET TAGS FOR A SPECIFIC FILE
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
            data["count"] = static_cast<int>(tags.size());
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // ADD TAG TO A FILE
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

    // REMOVE TAG FROM A FILE
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

    // GET ALL FILES WITH A SPECIFIC TAG
    CROW_ROUTE(app, "/tagger/files-by-tag")
    .methods("GET"_method)
    ([](const crow::request& request)
    {
        try
        {
            const char* tagRaw = request.url_params.get("tag");
            if (!tagRaw || std::string(tagRaw).empty())
            {
                return makeTaggerError(400, "No tag provided in request");
            }

            FileTagger tagger(requireDBPath());
            const auto files = tagger.getFilesForTag(std::string(tagRaw));

            crow::json::wvalue::list fileList;
            for (const auto& file : files)
            {
                fileList.emplace_back(file);
            }

            crow::json::wvalue data;
            data["tag"] = std::string(tagRaw);
            data["files"] = std::move(fileList);
            data["count"] = static_cast<int>(files.size());
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // GET ALL TAGS IN THE SYSTEM
    CROW_ROUTE(app, "/tagger/all-tags")
    .methods("GET"_method)
    ([](const crow::request&)
    {
        try
        {
            FileTagger tagger(requireDBPath());
            const auto allTags = tagger.getAllTags();

            crow::json::wvalue::list tagList;
            for (const auto& tag : allTags)
            {
                tagList.emplace_back(tag);
            }

            crow::json::wvalue data;
            data["tags"] = std::move(tagList);
            data["count"] = static_cast<int>(allTags.size());
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // ==================== DYNAMIC TAGGING ENDPOINTS ====================

    // APPLY RULES TO SINGLE FILE
    CROW_ROUTE(app, "/tagger/apply-rules")
    .methods("POST"_method)
    ([](const crow::request& request)
    {
        try
        {
            if (!g_dynamicTagger)
            {
                return makeTaggerError(500, "Dynamic tagger not initialized");
            }

            const auto body = crow::json::load(request.body);
            if (!body)
            {
                return makeTaggerError(400, "Invalid JSON body");
            }

            if (!body.has("path"))
            {
                return makeTaggerError(400, "Missing 'path' in request body");
            }

            const std::string path = body["path"].s();

            if (path.empty())
            {
                return makeTaggerError(400, "Path cannot be empty");
            }

            if (!std::filesystem::exists(path))
            {
                return makeTaggerError(404, "File does not exist");
            }

            g_dynamicTagger->applyRulesToFile(std::filesystem::path(path));

            crow::json::wvalue data;
            data["path"] = path;
            data["applied"] = true;
            data["message"] = "Rules applied to file";
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // APPLY RULES TO MULTIPLE FILES (BATCH)
    CROW_ROUTE(app, "/tagger/apply-rules-batch")
    .methods("POST"_method)
    ([](const crow::request& request)
    {
        try
        {
            if (!g_dynamicTagger)
            {
                return makeTaggerError(500, "Dynamic tagger not initialized");
            }

            const auto body = crow::json::load(request.body);
            if (!body)
            {
                return makeTaggerError(400, "Invalid JSON body");
            }

            if (!body.has("paths"))
            {
                return makeTaggerError(400, "Missing 'paths' array in request body");
            }

            const auto& pathsArray = body["paths"];
            std::vector<std::filesystem::path> paths;

            for (const auto& pathValue : pathsArray)
            {
                const std::string pathStr = pathValue.s();
                if (std::filesystem::exists(pathStr))
                {
                    paths.emplace_back(pathStr);
                }
            }

            if (paths.empty())
            {
                return makeTaggerError(400, "No valid file paths provided");
            }

            g_dynamicTagger->applyRulesToFiles(paths);

            crow::json::wvalue data;
            data["filesProcessed"] = static_cast<int>(paths.size());
            data["applied"] = true;
            data["message"] = "Rules applied to all files";
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // GET ALL AVAILABLE RULES
    CROW_ROUTE(app, "/tagger/rules")
    .methods("GET"_method)
    ([](const crow::request&)
    {
        try
        {
            if (!g_dynamicTagger)
            {
                return makeTaggerError(500, "Dynamic tagger not initialized");
            }

            const auto rules = g_dynamicTagger->getAllRules();

            crow::json::wvalue::list rulesList;
            for (const auto& rule : rules)
            {
                crow::json::wvalue ruleObj;
                ruleObj["name"] = rule.ruleName;
                ruleObj["enabled"] = rule.enabled;

                crow::json::wvalue::list tagsList;
                for (const auto& tag : rule.tags)
                {
                    tagsList.emplace_back(tag);
                }
                ruleObj["tags"] = std::move(tagsList);

                rulesList.emplace_back(std::move(ruleObj));
            }

            crow::json::wvalue data;
            data["rules"] = std::move(rulesList);
            data["count"] = static_cast<int>(rules.size());
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // ==================== BULK OPERATIONS ====================

    // ADD TAG TO MULTIPLE FILES
    CROW_ROUTE(app, "/tagger/add-tag-multiple")
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

            if (!body.has("paths") || !body.has("tag"))
            {
                return makeTaggerError(400, "Missing 'paths' array or 'tag' in request body");
            }

            const std::string tag = body["tag"].s();
            if (tag.empty())
            {
                return makeTaggerError(400, "Tag cannot be empty");
            }

            std::vector<std::string> validPaths;
            const auto& pathsArray = body["paths"];
            for (const auto& pathValue : pathsArray)
            {
                const std::string pathStr = pathValue.s();
                if (std::filesystem::exists(pathStr))
                {
                    validPaths.emplace_back(pathStr);
                }
            }

            if (validPaths.empty())
            {
                return makeTaggerError(400, "No valid file paths provided");
            }

            FileTagger tagger(requireDBPath());
            tagger.addTagToMultipleFiles(validPaths, tag);

            crow::json::wvalue data;
            data["tag"] = tag;
            data["filesTagged"] = static_cast<int>(validPaths.size());
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // REMOVE TAG FROM MULTIPLE FILES
    CROW_ROUTE(app, "/tagger/remove-tag-multiple")
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

            if (!body.has("paths") || !body.has("tag"))
            {
                return makeTaggerError(400, "Missing 'paths' array or 'tag' in request body");
            }

            const std::string tag = body["tag"].s();
            if (tag.empty())
            {
                return makeTaggerError(400, "Tag cannot be empty");
            }

            std::vector<std::string> validPaths;
            const auto& pathsArray = body["paths"];
            for (const auto& pathValue : pathsArray)
            {
                const std::string pathStr = pathValue.s();
                if (std::filesystem::exists(pathStr))
                {
                    validPaths.emplace_back(pathStr);
                }
            }

            if (validPaths.empty())
            {
                return makeTaggerError(400, "No valid file paths provided");
            }

            FileTagger tagger(requireDBPath());
            tagger.removeTagFromMultipleFiles(validPaths, tag);

            crow::json::wvalue data;
            data["tag"] = tag;
            data["filesUntagged"] = static_cast<int>(validPaths.size());
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // ==================== TAG MANAGEMENT ====================

    // DELETE TAG
    CROW_ROUTE(app, "/tagger/delete-tag")
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

            if (!body.has("tag"))
            {
                return makeTaggerError(400, "Missing 'tag' in request body");
            }

            const std::string tag = body["tag"].s();
            if (tag.empty())
            {
                return makeTaggerError(400, "Tag cannot be empty");
            }

            FileTagger tagger(requireDBPath());
            tagger.deleteTag(tag);

            crow::json::wvalue data;
            data["tag"] = tag;
            data["deleted"] = true;
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // RENAME TAG
    CROW_ROUTE(app, "/tagger/rename-tag")
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

            if (!body.has("oldTag") || !body.has("newTag"))
            {
                return makeTaggerError(400, "Missing 'oldTag' or 'newTag' in request body");
            }

            const std::string oldTag = body["oldTag"].s();
            const std::string newTag = body["newTag"].s();

            if (oldTag.empty() || newTag.empty())
            {
                return makeTaggerError(400, "Tags cannot be empty");
            }

            FileTagger tagger(requireDBPath());
            tagger.renameTag(oldTag, newTag);

            crow::json::wvalue data;
            data["oldTag"] = oldTag;
            data["newTag"] = newTag;
            data["renamed"] = true;
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // CHECK IF TAG EXISTS
    CROW_ROUTE(app, "/tagger/tag-exists")
    .methods("GET"_method)
    ([](const crow::request& request)
    {
        try
        {
            const char* tagRaw = request.url_params.get("tag");
            if (!tagRaw || std::string(tagRaw).empty())
            {
                return makeTaggerError(400, "No tag provided in request");
            }

            FileTagger tagger(requireDBPath());
            const bool exists = tagger.tagExists(std::string(tagRaw));

            crow::json::wvalue data;
            data["tag"] = std::string(tagRaw);
            data["exists"] = exists;
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });

    // GET FILE COUNT FOR TAG
    CROW_ROUTE(app, "/tagger/tag-file-count")
    .methods("GET"_method)
    ([](const crow::request& request)
    {
        try
        {
            const char* tagRaw = request.url_params.get("tag");
            if (!tagRaw || std::string(tagRaw).empty())
            {
                return makeTaggerError(400, "No tag provided in request");
            }

            FileTagger tagger(requireDBPath());
            const int count = tagger.getFileCountForTag(std::string(tagRaw));

            crow::json::wvalue data;
            data["tag"] = std::string(tagRaw);
            data["fileCount"] = count;
            return makeTaggerSuccess(std::move(data));
        }
        catch (const std::exception& e)
        {
            return makeTaggerError(500, e.what());
        }
    });
}

#endif //PALETTE_TAGGERAPI_H