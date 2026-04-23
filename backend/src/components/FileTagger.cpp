/******************************************
*   Name:           FileTagger.cpp
*   Description:    Contains functions for tagging files in the palette system
*   Author(s):      Hayden McVay <hm68s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/
#include "FileTagger.h"

#include <filesystem>
#include <iostream>
#include <sqlite3.h>
#include <stdexcept>

namespace fs = std::filesystem;

/**
 * @brief Initialize the tag manager and open its backing SQLite database.
 *
 * @param path path to the SQLite database file
 */
FileTagger::FileTagger(std::string path) : dbPath(std::move(path)), db(nullptr) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        const std::string error = db ? sqlite3_errmsg(db) : "unknown error";
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
        throw std::runtime_error("Failed to open tag database: " + error); //pathless dbs get thrown
    }

    // Initialize the database schema with required tables
    initializeDatabase();
}

/**
 * @brief Close the SQLite database connection if it is open.
 */
FileTagger::~FileTagger() {
    // Properly close the SQLite database connection if it exists
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

/**
 * @brief Create required tables for file, tag, and file-tag mappings.
 */
void FileTagger::initializeDatabase() {
    const char* createSql = R"(
    PRAGMA foreign_keys = ON;

    CREATE TABLE IF NOT EXISTS files (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        path TEXT NOT NULL UNIQUE
    );

    CREATE TABLE IF NOT EXISTS tags (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE
    );

    CREATE TABLE IF NOT EXISTS file_tags (
        file_id INTEGER NOT NULL,
        tag_id INTEGER NOT NULL,
        PRIMARY KEY (file_id, tag_id),
        FOREIGN KEY (file_id) REFERENCES files(id) ON DELETE CASCADE,
        FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE
    );
)";


    // Error handling for db initialization
    char* errorMessage = nullptr;
    if (sqlite3_exec(db, createSql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        const std::string error = errorMessage ? errorMessage : "unknown error";
        sqlite3_free(errorMessage);
        throw std::runtime_error("Failed to initialize tag database: " + error);
    }
}

/**
 * @brief Retrieve a file id or create the file row if it does not exist.
 *
 * @param filePath full or relative file path stored in the database
 * @return integer id of the file row
 */
int FileTagger::getOrCreateFileId(const std::string& filePath) {
    sqlite3_stmt* stmt = nullptr;
    int fileId = -1; //important for error handling see lines 105-108

    // First, check if the file already exists in the database
    const char* selectSql = "SELECT id FROM files WHERE path = ?;";
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare file lookup statement.");
    }

    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        fileId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    // If the file was found, return its ID immediately
    if (fileId != -1) {
        return fileId;
    }

    // File does not exist, so create a new entry
    const char* insertSql = "INSERT INTO files(path) VALUES(?);";
    if (sqlite3_prepare_v2(db, insertSql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare file insert statement.");
    }

    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert file record.");
    }

    sqlite3_finalize(stmt);
    // Return the ID of the newly inserted file row
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

/**
 * @brief Retrieve a tag id or create the tag row if it does not exist.
 *
 * @param tag tag name stored in the database
 * @return integer id of the tag row
 */
int FileTagger::getOrCreateTagId(const std::string& tag) {
    sqlite3_stmt* stmt = nullptr;
    int tagId = -1;

    // First, check if the tag already exists in the database
    const char* selectSql = "SELECT id FROM tags WHERE name = ?;";
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag lookup statement.");
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        tagId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    // If the tag was found, return its ID immediately
    if (tagId != -1) {
        return tagId;
    }

    // Tag does not exist, so create a new entry
    const char* insertSql = "INSERT INTO tags(name) VALUES(?);";
    if (sqlite3_prepare_v2(db, insertSql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag insert statement.");
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert tag record.");
    }

    sqlite3_finalize(stmt);
    // Return the ID of the newly inserted tag row
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

/**
 * @brief Print usage help for one command, or list all supported commands.
 *
 * @param command command name to describe; any unknown value prints command list
 */
void FileTagger::help(const std::string& command) {
    if (command == "addTag") {
        std::cout << "Usage: addTag <filePath> <tag>\nAdds a tag to the specified file." << std::endl;
    } else if (command == "removeTag") {
        std::cout << "Usage: removeTag <filePath> <tag>\nRemoves a tag from the specified file." << std::endl;
    } else if (command == "addFileToTag") {
        std::cout << "Usage: addFileToTag <tag> <filePath>\nAdds a file to the specified tag." << std::endl;
    } else if (command == "removeFileFromTag") {
        std::cout << "Usage: removeFileFromTag <tag> <filePath>\nRemoves a file from the specified tag." << std::endl;
    } else if (command == "getTagsForFile") {
        std::cout << "Usage: getTagsForFile <filePath>\nReturns a list of tags for the specified file." << std::endl;
    } else if (command == "verifyTagData") {
        std::cout << "Usage: verifyTagData <tagFolder>\nVerifies that all files in the tag data exist in the specified tag folder." << std::endl;
    } else {
        std::cout << "Available commands: addTag, removeTag, addFileToTag, removeFileFromTag, getTagsForFile, verifyTagData" << std::endl;
    }
}

/**
 * @brief Associate a tag with a file.
 *
 * Creates missing file/tag rows as needed and inserts into the join table.
 *
 * @param filePath file path to tag
 * @param tag tag name to apply
 */
void FileTagger::addTag(const std::string& filePath, const std::string& tag) {
    // Get or create the file ID from the database
    const int fileId = getOrCreateFileId(filePath);
    // Get or create the tag ID from the database
    const int tagId = getOrCreateTagId(tag);

    sqlite3_stmt* stmt = nullptr;
    // Use INSERT OR IGNORE to prevent duplicate file-tag associations
    const char* sql = "INSERT OR IGNORE INTO file_tags(file_id, tag_id) VALUES(?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare file_tag insert statement.");
    }

    // Bind the file ID and tag ID to the SQL statement
    sqlite3_bind_int(stmt, 1, fileId);
    sqlite3_bind_int(stmt, 2, tagId);

    // Execute the insert statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert file_tag record.");
    }

    // Clean up the prepared statement
    sqlite3_finalize(stmt);
}

/**
 * @brief Remove a file-tag association if present.
 *
 * @param filePath file path to untag
 * @param tag tag name to remove
 */
void FileTagger::removeTag(const std::string& filePath, const std::string& tag) {
    sqlite3_stmt* stmt = nullptr;
    // SQL query to delete the file-tag association using subqueries to find the IDs
    const char* sql = R"(
        DELETE FROM file_tags
        WHERE file_id = (SELECT id FROM files WHERE path = ?)
          AND tag_id = (SELECT id FROM tags WHERE name = ?);
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare file_tag delete statement.");
    }

    // Bind the file path and tag name to the SQL statement
    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, tag.c_str(), -1, SQLITE_TRANSIENT);

    // Execute the delete statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to remove file tag.");
    }

    // Clean up the prepared statement
    sqlite3_finalize(stmt);
}

/**
 * @brief Associate a file with a tag using tag-first argument order.
 *
 * @param tag tag name to apply
 * @param filePath file path to tag
 */
void FileTagger::addFileToTag(const std::string& tag, const std::string& filePath) {
    addTag(filePath, tag);
}

/**
 * @brief Remove a file-tag association using tag-first argument order.
 *
 * @param tag tag name to remove
 * @param filePath file path to untag
 */
void FileTagger::removeFileFromTag(const std::string& tag, const std::string& filePath) {
    removeTag(filePath, tag);
}

/**
 * @brief Fetch all tags associated with a file.
 *
 * @param filePath file path to query
 * @return sorted list of tag names, or empty list when none exist
 */
std::vector<std::string> FileTagger::getTagsForFile(const std::string& filePath) {
    std::vector<std::string> tags;
    sqlite3_stmt* stmt = nullptr;
    // SQL query to retrieve all tags for a specific file, using INNER JOINs to connect related tables
    const char* sql = R"(
        SELECT t.name
        FROM tags t
        INNER JOIN file_tags ft ON ft.tag_id = t.id
        INNER JOIN files f ON f.id = ft.file_id
        WHERE f.path = ?
        ORDER BY t.name;
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare get-tags statement.");
    }

    // Bind the file path to the SQL statement
    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);

    // Execute the query and collect all matching tags
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(stmt, 0);
        if (value) {
            tags.emplace_back(reinterpret_cast<const char*>(value));
        }
    }

    // Clean up the prepared statement
    sqlite3_finalize(stmt);
    return tags;
}

/**
 * @brief Validate that file paths stored in the database exist on disk.
 *
 * Writes warnings to stderr for missing files.
 *
 * @param tagFolder base folder used to resolve stored file paths
 */
void FileTagger::verifyTagData(const std::string& tagFolder) {
    sqlite3_stmt* stmt = nullptr;
    // SQL query to retrieve all file paths stored in the database
    const char* sql = "SELECT path FROM files;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag verification statement.");
    }

    // Iterate through all files in the database and check if they exist on disk
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(stmt, 0);
        if (!value) {
            continue;
        }

        const std::string filePath = reinterpret_cast<const char*>(value);
        // Construct the full path by combining the tag folder with the relative file path
        fs::path fullPath = fs::path(tagFolder) / filePath;

        // Check if the file exists; if not, print a warning message
        if (!fs::exists(fullPath)) {
            std::cerr << "Warning: File " << fullPath << " does not exist." << std::endl;
        }
    }

    // Clean up the prepared statement
    sqlite3_finalize(stmt);
}