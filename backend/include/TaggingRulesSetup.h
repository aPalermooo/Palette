/******************************************
*   Name:           TaggingRulesSetup.h
*   Description:    Setup function for all dynamic tagging rules
*   Author(s):      Hayden McVay <hm68s@missouristate.edu>
*   Date:           April 2026
*
*   Course:         CSC450
******************************************/
#ifndef PALETTE_TAGGINGRULESSETUP_H
#define PALETTE_TAGGINGRULESSETUP_H

#include "DynamicTagger.h"

/**
 * Sets up all default tagging rules for common file types
 * @param dynamicTagger Reference to DynamicTagger instance to configure
 */
inline void setupDynamicTaggingRules(DynamicTagger& dynamicTagger) {

    // ==================== DOCUMENTS ====================
    dynamicTagger.addRule("pdf-documents", R"(.*\.pdf$)", {"documents", "archive"});
    dynamicTagger.addRule("word-documents", R"(.*\.(doc|docx)$)", {"documents", "text", "editable"});
    dynamicTagger.addRule("text-documents", R"(.*\.txt$)", {"documents", "text"});
    dynamicTagger.addRule("spreadsheets", R"(.*\.(xls|xlsx|csv)$)", {"documents", "data", "spreadsheet"});
    dynamicTagger.addRule("presentations", R"(.*\.(ppt|pptx)$)", {"documents", "presentation"});
    dynamicTagger.addRule("pages-documents", R"(.*\.pages$)", {"documents", "text", "apple"});
    dynamicTagger.addRule("numbers-spreadsheets", R"(.*\.numbers$)", {"documents", "data", "apple"});
    dynamicTagger.addRule("keynote-presentations", R"(.*\.key$)", {"documents", "presentation", "apple"});

    // ==================== IMAGES ====================
    dynamicTagger.addRule("jpeg-images", R"(.*\.(jpg|jpeg)$)", {"media", "image", "photo"});
    dynamicTagger.addRule("png-images", R"(.*\.png$)", {"media", "image", "graphics"});
    dynamicTagger.addRule("gif-images", R"(.*\.gif$)", {"media", "image", "animation"});
    dynamicTagger.addRule("bitmap-images", R"(.*\.bmp$)", {"media", "image"});
    dynamicTagger.addRule("svg-images", R"(.*\.svg$)", {"media", "image", "vector"});
    dynamicTagger.addRule("webp-images", R"(.*\.webp$)", {"media", "image", "compressed"});
    dynamicTagger.addRule("tiff-images", R"(.*\.tiff?$)", {"media", "image", "archive"});
    dynamicTagger.addRule("ico-images", R"(.*\.ico$)", {"media", "image", "icon"});

    // ==================== AUDIO ====================
    dynamicTagger.addRule("mp3-audio", R"(.*\.mp3$)", {"media", "audio", "music", "compressed"});
    dynamicTagger.addRule("wav-audio", R"(.*\.wav$)", {"media", "audio", "music", "uncompressed"});
    dynamicTagger.addRule("flac-audio", R"(.*\.flac$)", {"media", "audio", "music", "lossless"});
    dynamicTagger.addRule("aac-audio", R"(.*\.aac$)", {"media", "audio", "music", "compressed"});
    dynamicTagger.addRule("ogg-audio", R"(.*\.ogg$)", {"media", "audio", "music", "open-source"});
    dynamicTagger.addRule("m4a-audio", R"(.*\.m4a$)", {"media", "audio", "music", "itunes"});
    dynamicTagger.addRule("wma-audio", R"(.*\.wma$)", {"media", "audio", "music", "windows"});
    dynamicTagger.addRule("opus-audio", R"(.*\.opus$)", {"media", "audio", "music", "compressed"});

    // ==================== VIDEO ====================
    dynamicTagger.addRule("mp4-video", R"(.*\.mp4$)", {"media", "video", "movie", "standard"});
    dynamicTagger.addRule("avi-video", R"(.*\.avi$)", {"media", "video", "movie", "windows"});
    dynamicTagger.addRule("mkv-video", R"(.*\.mkv$)", {"media", "video", "movie", "container"});
    dynamicTagger.addRule("mov-video", R"(.*\.mov$)", {"media", "video", "movie", "apple"});
    dynamicTagger.addRule("flv-video", R"(.*\.flv$)", {"media", "video", "movie", "flash"});
    dynamicTagger.addRule("wmv-video", R"(.*\.wmv$)", {"media", "video", "movie", "windows"});
    dynamicTagger.addRule("webm-video", R"(.*\.webm$)", {"media", "video", "movie", "web"});
    dynamicTagger.addRule("m4v-video", R"(.*\.m4v$)", {"media", "video", "movie", "itunes"});
    dynamicTagger.addRule("mpg-video", R"(.*\.mpg|.*\.mpeg$)", {"media", "video", "movie", "standard"});
    dynamicTagger.addRule("3gp-video", R"(.*\.3gp$)", {"media", "video", "mobile"});

    // ==================== ARCHIVES ====================
    dynamicTagger.addRule("zip-archive", R"(.*\.zip$)", {"archive", "compressed", "standard"});
    dynamicTagger.addRule("rar-archive", R"(.*\.rar$)", {"archive", "compressed"});
    dynamicTagger.addRule("7z-archive", R"(.*\.7z$)", {"archive", "compressed", "high-compression"});
    dynamicTagger.addRule("tar-archive", R"(.*\.tar$)", {"archive", "uncompressed"});
    dynamicTagger.addRule("gzip-archive", R"(.*\.tar\.gz|.*\.tgz$)", {"archive", "compressed", "unix"});
    dynamicTagger.addRule("bzip-archive", R"(.*\.tar\.bz2|.*\.tbz$)", {"archive", "compressed"});
    dynamicTagger.addRule("gz-archive", R"(.*\.gz$)", {"archive", "compressed"});
    dynamicTagger.addRule("bz2-archive", R"(.*\.bz2$)", {"archive", "compressed"});
    dynamicTagger.addRule("iso-image", R"(.*\.iso$)", {"archive", "disk-image"});
}

#endif // PALETTE_TAGGINGRULESSETUP_H