//
// Created by Cinnamon on 3/9/2026.
//

#include "explorer/Handler.h"
#include "explorer/FileHandler.h"
#include "explorer/DirectoryHandler.h"

#include <filesystem>

Handler* Handler::getHandler(const std::filesystem::path& path) {
    if (is_directory(path))
        return new DirectoryHandler();
    return new FileHandler();
}
