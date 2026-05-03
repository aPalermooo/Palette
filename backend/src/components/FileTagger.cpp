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

FileTagger::~FileTagger() {
    // Properly close the SQLite database connection if it exists
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

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

    CREATE INDEX IF NOT EXISTS idx_tags_name ON tags(name);
    CREATE INDEX IF NOT EXISTS idx_files_path ON files(path);
    CREATE INDEX IF NOT EXISTS idx_file_tags_tag_id ON file_tags(tag_id);
    CREATE INDEX IF NOT EXISTS idx_file_tags_file_id ON file_tags(file_id);
)";

    // Error handling for db initialization
    char* errorMessage = nullptr;
    if (sqlite3_exec(db, createSql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        const std::string error = errorMessage ? errorMessage : "unknown error";
        sqlite3_free(errorMessage);
        throw std::runtime_error("Failed to initialize tag database: " + error);
    }
}

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

void FileTagger::addFileToTag(const std::string& tag, const std::string& filePath) {
    addTag(filePath, tag);
}

void FileTagger::removeFileFromTag(const std::string& tag, const std::string& filePath) {
    removeTag(filePath, tag);
}

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

std::vector<std::string> FileTagger::getFilesForTag(const std::string& tag) {
    std::vector<std::string> files;
    sqlite3_stmt* stmt = nullptr;

    const char* sql = R"(
        SELECT f.path
        FROM files f
        INNER JOIN file_tags ft ON ft.file_id = f.id
        INNER JOIN tags t ON t.id = ft.tag_id
        WHERE t.name = ?
        ORDER BY f.path;
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare files-by-tag query statement.");
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(stmt, 0);
        if (value) {
            files.emplace_back(reinterpret_cast<const char*>(value));
        }
    }

    sqlite3_finalize(stmt);
    return files;
}

std::vector<std::string> FileTagger::getAllTags() {
    std::vector<std::string> tags;
    sqlite3_stmt* stmt = nullptr;

    const char* sql = "SELECT name FROM tags ORDER BY name;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare all-tags query statement.");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(stmt, 0);
        if (value) {
            tags.emplace_back(reinterpret_cast<const char*>(value));
        }
    }

    sqlite3_finalize(stmt);
    return tags;
}

void FileTagger::addTagToMultipleFiles(const std::vector<std::string>& filePaths, const std::string& tag) {
    if (filePaths.empty()) {
        throw std::runtime_error("File paths vector cannot be empty.");
    }

    // Get the tag ID once for efficiency
    const int tagId = getOrCreateTagId(tag);

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT OR IGNORE INTO file_tags(file_id, tag_id) VALUES(?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare bulk tag insert statement.");
    }

    for (const auto& filePath : filePaths) {
        const int fileId = getOrCreateFileId(filePath);

        sqlite3_bind_int(stmt, 1, fileId);
        sqlite3_bind_int(stmt, 2, tagId);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to insert file_tag record for: " + filePath);
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
}

void FileTagger::removeTagFromMultipleFiles(const std::vector<std::string>& filePaths, const std::string& tag) {
    if (filePaths.empty()) {
        throw std::runtime_error("File paths vector cannot be empty.");
    }

    sqlite3_stmt* stmt = nullptr;
    const char* sql = R"(
        DELETE FROM file_tags
        WHERE file_id = (SELECT id FROM files WHERE path = ?)
          AND tag_id = (SELECT id FROM tags WHERE name = ?);
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare bulk tag delete statement.");
    }

    for (const auto& filePath : filePaths) {
        sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, tag.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to remove tag from file: " + filePath);
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
}

void FileTagger::deleteTag(const std::string& tag) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM tags WHERE name = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag delete statement.");
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to delete tag: " + tag);
    }

    sqlite3_finalize(stmt);
}

void FileTagger::renameTag(const std::string& oldTag, const std::string& newTag) {
    if (oldTag.empty() || newTag.empty()) {
        throw std::runtime_error("Tag names cannot be empty.");
    }

    if (oldTag == newTag) {
        return; // No-op if names are the same
    }

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE tags SET name = ? WHERE name = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag rename statement.");
    }

    sqlite3_bind_text(stmt, 1, newTag.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, oldTag.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to rename tag from '" + oldTag + "' to '" + newTag + "'");
    }

    sqlite3_finalize(stmt);
}

bool FileTagger::tagExists(const std::string& tag) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id FROM tags WHERE name = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag existence check statement.");
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);

    const bool exists = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);

    return exists;
}

int FileTagger::getFileCountForTag(const std::string& tag) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = R"(
        SELECT COUNT(f.id)
        FROM files f
        INNER JOIN file_tags ft ON ft.file_id = f.id
        INNER JOIN tags t ON t.id = ft.tag_id
        WHERE t.name = ?;
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare file count query.");
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}