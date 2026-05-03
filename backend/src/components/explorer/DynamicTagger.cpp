/******************************************
*   Name:           DynamicTagger.cpp
*   Description:    Implementation of regex-based dynamic tagging
*   Author(s):      Hayden McVay <hm68s@missouristate.edu>
*   Date:           April 2026
*
*   Course:         CSC450
******************************************/
#include "DynamicTagger.h"
#include <algorithm>
#include <iostream>

DynamicTagger::DynamicTagger(FileTagger& fileTagger) : tagger(fileTagger) {}

void DynamicTagger::addRule(const std::string& ruleName, const std::string& regexPattern,
                             const std::vector<std::string>& tagsToApply) {
    if (ruleName.empty() || regexPattern.empty() || tagsToApply.empty()) {
        throw std::runtime_error("Rule name, pattern, and tags cannot be empty.");
    }

    if (ruleExists(ruleName)) {
        throw std::runtime_error("Rule '" + ruleName + "' already exists.");
    }

    try {
        TagRule newRule;
        newRule.ruleName = ruleName;
        newRule.pattern = std::regex(regexPattern);
        newRule.tags = tagsToApply;
        newRule.enabled = true;

        rules.emplace_back(std::move(newRule));
    } catch (const std::regex_error& e) {
        throw std::runtime_error("Invalid regex pattern: " + std::string(e.what()));
    }
}

void DynamicTagger::removeRule(const std::string& ruleName) {
    auto it = std::find_if(rules.begin(), rules.end(),
        [&ruleName](const TagRule& rule) { return rule.ruleName == ruleName; });

    if (it == rules.end()) {
        throw std::runtime_error("Rule '" + ruleName + "' not found.");
    }

    rules.erase(it);
}

void DynamicTagger::applyRulesToFile(const std::filesystem::path& filePath) {
    const std::string filePathStr = filePath.string();

    for (const auto& rule : rules) {
        if (!rule.enabled) continue;

        if (std::regex_search(filePathStr, rule.pattern)) {
            for (const auto& tag : rule.tags) {
                try {
                    tagger.addTag(filePathStr, tag);
                } catch (const std::exception& e) {
                    std::cerr << "Error applying tag '" << tag << "' to file '"
                              << filePathStr << "': " << e.what() << std::endl;
                }
            }
        }
    }
}

void DynamicTagger::applyRulesToFiles(const std::vector<std::filesystem::path>& filePaths) {
    for (const auto& filePath : filePaths) {
        applyRulesToFile(filePath);
    }
}

void DynamicTagger::enableRule(const std::string& ruleName) {
    auto it = std::find_if(rules.begin(), rules.end(),
        [&ruleName](TagRule& rule) { return rule.ruleName == ruleName; });

    if (it == rules.end()) {
        throw std::runtime_error("Rule '" + ruleName + "' not found.");
    }

    it->enabled = true;
}

void DynamicTagger::disableRule(const std::string& ruleName) {
    auto it = std::find_if(rules.begin(), rules.end(),
        [&ruleName](TagRule& rule) { return rule.ruleName == ruleName; });

    if (it == rules.end()) {
        throw std::runtime_error("Rule '" + ruleName + "' not found.");
    }

    it->enabled = false;
}

std::vector<TagRule> DynamicTagger::getAllRules() const {
    return rules;
}

bool DynamicTagger::ruleExists(const std::string& ruleName) const {
    return std::any_of(rules.begin(), rules.end(),
        [&ruleName](const TagRule& rule) { return rule.ruleName == ruleName; });
}