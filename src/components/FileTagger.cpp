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

FileTagger::FileTagger(const std::string& path) : dbPath(path), db(nullptr) { //string should eventually be replaced with contents of a dir
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        const std::string error = db ? sqlite3_errmsg(db) : "unknown error";
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
        throw std::runtime_error("Failed to open tag database: " + error);
    }

    initializeDatabase();
}

FileTagger::~FileTagger() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

/* INITIALIZE DATABASE */
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

    char* errorMessage = nullptr;
    if (sqlite3_exec(db, createSql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        const std::string error = errorMessage ? errorMessage : "unknown error";
        sqlite3_free(errorMessage);
        throw std::runtime_error("Failed to initialize tag database: " + error);
    }
}

int FileTagger::getOrCreateFileId(const std::string& filePath) {
    sqlite3_stmt* stmt = nullptr;
    int fileId = -1;

    const char* selectSql = "SELECT id FROM files WHERE path = ?;";
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare file lookup statement.");
    }

    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        fileId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (fileId != -1) {
        return fileId;
    }

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
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

int FileTagger::getOrCreateTagId(const std::string& tag) {
    sqlite3_stmt* stmt = nullptr;
    int tagId = -1;

    const char* selectSql = "SELECT id FROM tags WHERE name = ?;";
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag lookup statement.");
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        tagId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (tagId != -1) {
        return tagId;
    }

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
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

/* HELP FUNCTION */
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

/* ADD TAG TO FILE */
void FileTagger::addTag(const std::string& filePath, const std::string& tag) {
    const int fileId = getOrCreateFileId(filePath);
    const int tagId = getOrCreateTagId(tag);

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT OR IGNORE INTO file_tags(file_id, tag_id) VALUES(?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare file_tag insert statement.");
    }

    sqlite3_bind_int(stmt, 1, fileId);
    sqlite3_bind_int(stmt, 2, tagId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert file_tag record.");
    }

    sqlite3_finalize(stmt);
}

/* REMOVE TAG FROM FILE */
void FileTagger::removeTag(const std::string& filePath, const std::string& tag) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = R"(
        DELETE FROM file_tags
        WHERE file_id = (SELECT id FROM files WHERE path = ?)
          AND tag_id = (SELECT id FROM tags WHERE name = ?);
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare file_tag delete statement.");
    }

    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, tag.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to remove file tag.");
    }

    sqlite3_finalize(stmt);
}

/* ADD FILE TO TAG */
void FileTagger::addFileToTag(const std::string& tag, const std::string& filePath) {
    addTag(filePath, tag);
}

/* REMOVE FILE FROM TAG */
void FileTagger::removeFileFromTag(const std::string& tag, const std::string& filePath) {
    removeTag(filePath, tag);
}

/* GET TAGS FOR FILE */
std::vector<std::string> FileTagger::getTagsForFile(const std::string& filePath) {
    std::vector<std::string> tags;
    sqlite3_stmt* stmt = nullptr;
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

    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(stmt, 0);
        if (value) {
            tags.emplace_back(reinterpret_cast<const char*>(value));
        }
    }

    sqlite3_finalize(stmt);
    return tags;
}

/* VERIFY TAG DATA */
void FileTagger::verifyTagData(const std::string& tagFolder) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT path FROM files;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare tag verification statement.");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(stmt, 0);
        if (!value) {
            continue;
        }

        const std::string filePath = reinterpret_cast<const char*>(value);
        fs::path fullPath = fs::path(tagFolder) / filePath;

        if (!fs::exists(fullPath)) {
            std::cerr << "Warning: File " << fullPath << " does not exist." << std::endl;
        }
    }

    sqlite3_finalize(stmt);
}

