/******************************************
*   Name:           Database.h
*   Description:    Manages state of connection to database
*   Author(s):      Xander Palermo <ajp2s@missouristate.edu>
*   Date:           March 2026
*
*   Course:         CSC450
******************************************/

#ifndef PALETTE_DATABASE_H
#define PALETTE_DATABASE_H
#include <sqlite3.h>
#include <filesystem>

/**
 * @class Database
 * @brief Manages connection state to SQLite Database
 *
 * @note cannot be copied - One instance to one database connection
 */
class Database {
private:
    sqlite3 *db;
public:

    /**
     * Creates a new database connection
     * @param dbPath Absolute path of database file
     */
    explicit Database(const std::filesystem::path& dbPath) {
        if (sqlite3_open(dbPath.string().c_str(), &db)!=SQLITE_OK) {
            throw std::runtime_error(std::string("Can't open database: ") + sqlite3_errmsg(db));
        }
    }

    /**
     * Close Database connection
     */
    ~Database() {
        if (db) sqlite3_close(db);
    }

    // No copying class; Only one connection at a time
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    /**
     * Get pointer to database connection handle
     * @return Raw pointer to database connection
     */
    sqlite3* get() const {return db;}

    /**
     * Checks if database is currently connected
     * @return True is database is connected, otherwise False
     */
    bool isOpen() const {return db != nullptr;}
};

#endif //PALETTE_DATABASE_H