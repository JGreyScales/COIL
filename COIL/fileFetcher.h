#pragma once

#define ASIO_STANDALONE

#include "crow_all.h"
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <vector>

class fileFetcher
{
public:
    inline std::string sendFile(std::string filename, std::string directory = "", bool publicFolder = true)
    {
        namespace fs = std::filesystem;

        std::vector<fs::path> candidatePaths;
        fs::path cwd = fs::current_path();

        // If caller provided a custom directory, try that first.
        if (!directory.empty())
        {
            candidatePaths.push_back(cwd / directory / filename);
            candidatePaths.push_back(cwd.parent_path() / directory / filename);
            candidatePaths.push_back(cwd.parent_path().parent_path() / directory / filename);
        }

        // Default public-folder search paths
        if (publicFolder)
        {
            // current behavior equivalents
            candidatePaths.push_back(cwd / "public" / filename);
            candidatePaths.push_back(cwd / "public" / "src" / filename);

            // account for running from /shared/build
            candidatePaths.push_back(cwd.parent_path() / "public" / filename);
            candidatePaths.push_back(cwd.parent_path() / "public" / "src" / filename);

            // one more level up just in case build is nested
            candidatePaths.push_back(cwd.parent_path().parent_path() / "public" / filename);
            candidatePaths.push_back(cwd.parent_path().parent_path() / "public" / "src" / filename);
        }
        else
        {
            candidatePaths.push_back(cwd / filename);
            candidatePaths.push_back(cwd.parent_path() / filename);
            candidatePaths.push_back(cwd.parent_path().parent_path() / filename);
        }

        std::cout << "Current working directory: " << cwd << "\n";
        std::cout << "Trying to find file: " << filename << "\n";

        for (const auto& path : candidatePaths)
        {
            std::cout << "Attempting to open: " << path << "\n";

            std::ifstream in(path, std::ios::in | std::ios::binary);
            if (in.is_open())
            {
                std::ostringstream contents;
                contents << in.rdbuf();
                in.close();

                std::cout << "SUCCESS: opened " << path << "\n";
                return contents.str();
            }
        }

        std::cout << "ERROR: file not found in any candidate path.\n";
        return "not found";
    }
};