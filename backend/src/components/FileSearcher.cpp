/***********************************
 *  Name:           FileSearcher.cpp
 *  Description:    Implementation of FileSearcher - handles file searching by tags
 *  Author(s):      Hayden McVay <hm68s@missouristate.edu>
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/

#include "FileSearcher.h"

#include <algorithm>
#include <sqlite3.h>
#include <stdexcept>

FileSearcher::FileSearcher(std::string path) : dbPath(std::move(path)), db(nullptr) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        const std::string error = db ? sqlite3_errmsg(db) : "unknown error";
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
        throw std::runtime_error("Failed to open search database: " + error);
    }

    // Enable foreign keys needed for this searcher to function properly
    char* errorMessage = nullptr;
    if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        const std::string error = errorMessage ? errorMessage : "unknown error";
        sqlite3_free(errorMessage);
        throw std::runtime_error("Failed to enable foreign keys: " + error);
    }
}

FileSearcher::~FileSearcher() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

// Private

std::vector<std::string> FileSearcher::collectResults(sqlite3_stmt* stmt) {
    std::vector<std::string> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(stmt, 0);
        if (value) {
            results.emplace_back(reinterpret_cast<const char*>(value));
        }
    }
    sqlite3_finalize(stmt);
    return results;
}

std::string FileSearcher::buildPlaceholders(size_t count) {
    std::string placeholders;
    for (size_t i = 0; i < count; ++i) {
        placeholders += "?";
        if (i < count - 1) placeholders += ", ";
    }
    return placeholders;
}

// Public

std::vector<std::string> FileSearcher::getFilesWithTag(const std::string& tag) {
    if (tag.empty()) {
        throw std::invalid_argument("Tag cannot be empty");
    }

    sqlite3_stmt* stmt = nullptr;

    // SQL query to retrieve all files with a specific tag
    const char* sql = R"(
        SELECT f.path
        FROM files f
        INNER JOIN file_tags ft ON ft.file_id = f.id
        INNER JOIN tags t ON t.id = ft.tag_id
        WHERE t.name = ?
        ORDER BY f.path;
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare search statement.");
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);
    return collectResults(stmt);
}

std::vector<std::string> FileSearcher::searchByTag(const std::string& tag) {
    if (tag.empty()) {
        throw std::invalid_argument("Tag cannot be empty");
    }

    return getFilesWithTag(tag);
}


std::vector<std::string> FileSearcher::searchByMultipleTags(
    const std::vector<std::string>& tags,
    bool requireAll) {

    if (tags.empty()) {
        throw std::invalid_argument("Tags list cannot be empty");
    }

    sqlite3_stmt* stmt = nullptr;
    std::string placeholders = buildPlaceholders(tags.size());

    if (requireAll) {
        // Get files that have ALL specified tags
        std::string sql = R"(
            SELECT f.path
            FROM files f
            WHERE (
                SELECT COUNT(DISTINCT t.id)
                FROM file_tags ft
                INNER JOIN tags t ON t.id = ft.tag_id
                WHERE ft.file_id = f.id
                  AND t.name IN ()" + placeholders + R"()
            ) = ?
            ORDER BY f.path;
        )";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare multi-tag search statement (requireAll=true).");
        }

        // Bind all tag names
        for (size_t i = 0; i < tags.size(); ++i) {
            sqlite3_bind_text(stmt, i + 1, tags[i].c_str(), -1, SQLITE_TRANSIENT);
        }
        // Bind the count requirement
        sqlite3_bind_int(stmt, tags.size() + 1, static_cast<int>(tags.size()));

    } else {
        // Get files that have ANY of the specified tags
        std::string sql = R"(
            SELECT DISTINCT f.path
            FROM files f
            INNER JOIN file_tags ft ON ft.file_id = f.id
            INNER JOIN tags t ON t.id = ft.tag_id
            WHERE t.name IN ()" + placeholders + R"()
            ORDER BY f.path;
        )";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare multi-tag search statement (requireAll=false).");
        }

        // Bind all tag names
        for (size_t i = 0; i < tags.size(); ++i) {
            sqlite3_bind_text(stmt, i + 1, tags[i].c_str(), -1, SQLITE_TRANSIENT);
        }
    }

    return collectResults(stmt);
}

std::vector<std::string> FileSearcher::searchByTagsThenName(
    const std::vector<std::string>& tags,
    bool sortAscending) {

    if (tags.empty()) {
        throw std::invalid_argument("Tags list cannot be empty");
    }

    // Get all files that have any of the specified tags
    std::vector<std::string> results = searchByMultipleTags(tags, false);

    // Sort by filename (last component of path)
    std::sort(results.begin(), results.end(), [sortAscending](const std::string& a, const std::string& b) {
        // Extract filename from full path
        size_t posA = a.find_last_of("\\/");
        size_t posB = b.find_last_of("\\/");

        std::string nameA = (posA == std::string::npos) ? a : a.substr(posA + 1);
        std::string nameB = (posB == std::string::npos) ? b : b.substr(posB + 1);

        // Case-insensitive comparison
        std::transform(nameA.begin(), nameA.end(), nameA.begin(), ::tolower);
        std::transform(nameB.begin(), nameB.end(), nameB.begin(), ::tolower);

        return sortAscending ? nameA < nameB : nameA > nameB;
    });

    return results;
}

std::vector<std::string> FileSearcher::searchByFileNamePattern(const std::string& pattern) {
    if (pattern.empty()) {
        throw std::invalid_argument("Pattern cannot be empty");
    }

    sqlite3_stmt* stmt = nullptr;

    // SQL query using LIKE to match file paths/names
    const char* sql = R"(
        SELECT f.path
        FROM files f
        WHERE f.path LIKE ?
        ORDER BY f.path;
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare filename pattern search statement.");
    }

    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    return collectResults(stmt);
}

std::vector<std::string> FileSearcher::searchByTagPattern(const std::string& pattern) {
    if (pattern.empty()) {
        throw std::invalid_argument("Pattern cannot be empty");
    }

    sqlite3_stmt* stmt = nullptr;

    // SQL query to find files with tags matching the LIKE pattern
    const char* sql = R"(
        SELECT DISTINCT f.path
        FROM files f
        INNER JOIN file_tags ft ON ft.file_id = f.id
        INNER JOIN tags t ON t.id = ft.tag_id
        WHERE t.name LIKE ?
        ORDER BY f.path;
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag pattern search statement.");
    }

    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    return collectResults(stmt);
}

std::vector<std::string> FileSearcher::searchByNameAndTags(
    const std::string& namePattern,
    const std::vector<std::string>& tags,
    bool requireAllTags) {

    if (namePattern.empty() || tags.empty()) {
        throw std::invalid_argument("Pattern and tags cannot be empty");
    }

    sqlite3_stmt* stmt = nullptr;
    std::string placeholders = buildPlaceholders(tags.size());

    std::string sql = R"(
        SELECT DISTINCT f.path
        FROM files f
        INNER JOIN file_tags ft ON ft.file_id = f.id
        INNER JOIN tags t ON t.id = ft.tag_id
        WHERE f.path LIKE ?
          AND (
    )";

    if (requireAllTags) {
        sql += R"(
            SELECT COUNT(DISTINCT t2.id)
            FROM file_tags ft2
            INNER JOIN tags t2 ON t2.id = ft2.tag_id
            WHERE ft2.file_id = f.id
              AND t2.name IN ()" + placeholders + R"()
        ) = ?
        )";
    } else {
        sql += "t.name IN (" + placeholders + ")";
    }

    sql += " ORDER BY f.path;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare combined name and tag search statement.");
    }

    // Bind the name pattern
    sqlite3_bind_text(stmt, 1, namePattern.c_str(), -1, SQLITE_TRANSIENT);

    // Bind all tag names
    for (size_t i = 0; i < tags.size(); ++i) {
        sqlite3_bind_text(stmt, static_cast<int>(i + 2), tags[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    // If requireAllTags, bind the count requirement
    if (requireAllTags) {
        sqlite3_bind_int(stmt, static_cast<int>(tags.size() + 2), static_cast<int>(tags.size()));
    }

    return collectResults(stmt);
}