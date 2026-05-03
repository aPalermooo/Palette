/******************************************
*   Name:           DynamicTagger.h
*   Description:    Regex-based dynamic tagging system
*   Author(s):      Hayden McVay <hm68s@missouristate.edu>
*   Date:           April 2026
*
*   Course:         CSC450
******************************************/
#ifndef PALETTE_DYNAMICTAGGER_H
#define PALETTE_DYNAMICTAGGER_H

#include "FileTagger.h"
#include <string>
#include <vector>
#include <regex>
#include <filesystem>

struct TagRule {
    std::string ruleName;
    std::regex pattern;
    std::vector<std::string> tags;
    bool enabled;
};

class DynamicTagger {
private:
    FileTagger& tagger;
    std::vector<TagRule> rules;

public:
    explicit DynamicTagger(FileTagger& fileTagger);

    /**
     * Add a regex rule that applies multiple tags to matching files
     * @param ruleName Unique name for the rule
     * @param regexPattern Regex pattern to match file paths
     * @param tagsToApply Vector of tags to apply when pattern matches
     */
    void addRule(const std::string& ruleName, const std::string& regexPattern,
                 const std::vector<std::string>& tagsToApply);

    /**
     * Remove a rule by name
     * @param ruleName Name of the rule to remove
     */
    void removeRule(const std::string& ruleName);

    /**
     * Apply all enabled rules to a single file
     * @param filePath Path to the file to tag
     */
    void applyRulesToFile(const std::filesystem::path& filePath);

    /**
     * Apply all enabled rules to multiple files
     * @param filePaths Vector of file paths to tag
     */
    void applyRulesToFiles(const std::vector<std::filesystem::path>& filePaths);

    /**
     * Enable a rule
     * @param ruleName Name of the rule to enable
     */
    void enableRule(const std::string& ruleName);

    /**
     * Disable a rule
     * @param ruleName Name of the rule to disable
     */
    void disableRule(const std::string& ruleName);

    /**
     * Get all defined rules
     * @return Vector of all TagRules
     */
    [[nodiscard]] std::vector<TagRule> getAllRules() const;

    /**
     * Check if a rule exists
     * @param ruleName Name of the rule to check
     * @return true if rule exists, false otherwise
     */
    [[nodiscard]] bool ruleExists(const std::string& ruleName) const;
};

#endif // PALETTE_DYNAMICTAGGER_H