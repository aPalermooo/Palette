/***********************************
*  Name:           FileSearcher.h
 *  Description:    Provides search functionality for files stored in the
 *                  Palette database, supporting tag-based and pattern-based
 *                  queries.
 *
 *  Author(s):      Hayden McVay <hm68s@missouristate.edu>
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/

#ifndef PALETTE_FILESEARCHER_H
#define PALETTE_FILESEARCHER_H

#include <string>
#include <vector>
#include <sqlite3.h>

class FileSearcher {
private:
    std::string dbPath;
    sqlite3* db;

    /**
     * Retrieves all file paths from the database associated with a single tag.
     *
     * @param tag The tag name to search for
     * @pre tag is a non-empty string matching an existing tag in the database
     * @return A list of absolute file paths associated with the given tag
     */
    std::vector<std::string> getFilesWithTag(const std::string& tag);

public:
    /**
     * Opens a connection to the SQLite database at the specified path.
     *
     * @param path Absolute path to the SQLite database file
     * @pre path points to a valid, accessible SQLite database
     * @throws std::runtime_error if the database cannot be opened
     */
    explicit FileSearcher(std::string path);

    /**
     * Closes the SQLite database connection and releases all resources.
     */
    ~FileSearcher();

    // Delete copy operations (similar to FileTagger pattern)
    FileSearcher(const FileSearcher&) = delete;
    FileSearcher& operator=(const FileSearcher&) = delete;

    /**
     * Retrieves all files matching any of the provided tags, sorted by filename.
     *
     * @param tags List of tag names to search for
     * @param sortAscending When true, results are sorted A→Z; false sorts Z→A
     * @pre tags is non-empty
     * @return A list of absolute file paths sorted by filename
     * @throws std::invalid_argument if tags is empty
     */
    std::vector<std::string> searchByTagsThenName(
        const std::vector<std::string>& tags,
        bool sortAscending = true
    );

    /**
     * Retrieves all files associated with a single tag.
     *
     * @param tag The tag name to search for
     * @pre tag is a non-empty string
     * @return A list of absolute file paths that have the given tag
     * @throws std::invalid_argument if tag is empty
     */
    std::vector<std::string> searchByTag(const std::string& tag);

    /**
     * Retrieves files matching multiple tags, with optional AND/OR logic.
     *
     * @param tags List of tag names to search for
     * @param requireAll When true, only files with ALL tags are returned (AND);
     *                   when false, files with ANY matching tag are returned (OR)
     * @pre tags is non-empty
     * @return A list of absolute file paths matching the tag criteria
     * @throws std::invalid_argument if tags is empty
     */
    std::vector<std::string> searchByMultipleTags(
        const std::vector<std::string>& tags,
        bool requireAll = true
    );

    /**
     * Retrieves all files whose path matches a SQL LIKE pattern.
     *
     * @param pattern A SQL LIKE pattern string (e.g. "%.txt", "%report%")
     * @pre pattern is a non-empty string
     * @return A list of absolute file paths matching the given pattern
     * @throws std::invalid_argument if pattern is empty
     */
    std::vector<std::string> searchByFileNamePattern(const std::string& pattern);

    /**
     * Retrieves all files that have at least one tag matching a SQL LIKE pattern.
     *
     * @param pattern A SQL LIKE pattern string to match against tag names (e.g. "work%")
     * @pre pattern is a non-empty string
     * @return A list of absolute file paths whose tags match the given pattern
     * @throws std::invalid_argument if pattern is empty
     */
    std::vector<std::string> searchByTagPattern(const std::string& pattern);

    /**
     * Retrieves files matching both a filename pattern and a set of tags.
     *
     * @param namePattern A SQL LIKE pattern to match against file paths (e.g. "%report%")
     * @param tags List of tag names to filter results by
     * @param requireAllTags When true, only files with ALL tags are returned (AND);
     *                       when false, files with ANY matching tag are returned (OR)
     * @pre namePattern and tags are both non-empty
     * @return A list of absolute file paths satisfying both the name and tag criteria
     * @throws std::invalid_argument if namePattern or tags is empty
     */
    std::vector<std::string> searchByNameAndTags(
        const std::string& namePattern,
        const std::vector<std::string>& tags,
        bool requireAllTags = true
    );
private:
    // Helper to collect results from a prepared statement
    std::vector<std::string> collectResults(sqlite3_stmt* stmt);

    // Helper to build placeholder string for dynamic SQL
    std::string buildPlaceholders(size_t count);

};

#endif //PALETTE_FILESEARCHER_H
