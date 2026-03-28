#pragma once


#define ASIO_STANDALONE

#include "crow_all.h"
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>


class fileFetcher
{
public:
    inline std::string sendFile(std::string filename, std::string directory = "", bool publicFolder = true)
    {
        // allow overrides to the directory
        // but if nothing is provided use public/
        if (directory.length() > 1)
        {
            filename = directory + '/' + filename;
        }

        if (publicFolder) {
            filename = "public/" + filename;
        }

        
        std::cout << "Current working directory: " << std::filesystem::current_path() << "\n";
        std::cout << "Attempting to open: " << filename << "\n";

        // return the file contents as a c 
        std::ifstream in(filename);
        if (in)
        {
            std::ostringstream contents;
            contents << in.rdbuf();
            in.close();
            return contents.str();
        }

        return "not found";
    }
};