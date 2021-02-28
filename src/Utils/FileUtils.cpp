#include "Utils/FileUtils.hpp"
#include <dirent.h>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include "logging.hpp"

using namespace std;

namespace FileUtils
{
    string GetFileName(string path, bool removeExtension)
    {
        if (path.find('/')) path.erase(path.find_last_of('/'));
        if (removeExtension) path = remove_extension(path);
        return path;
    }

    string remove_extension(string path)
    {
        return path.erase(path.find_last_of('.') - 1);
    }

    string GetExtension(string path)
    {
        if(path.find_last_of(".") != std::string::npos)
            return path.substr(path.find_last_of(".")+1);
        return "";
    }

    bool GetFilesInFolderPath(std::string extension, std::string filePath, std::vector<std::string>& out)
    {
        INFO("Finding files in %s", filePath.c_str());
        bool foundTheExtension = false; 
        /*
        std::filesystem::path path(filePath);
        std::filesystem::directory_iterator dir(path);
        for (auto& p : dir)
        {
            std::string ext = GetExtension(p.path());
            if(ext.find(extension) != std::string::npos)
            {
                out.push_back(GetFileName(p.path()));
                foundTheExtension = true; 
            }
        }
        */
        
        DIR* fileDir = opendir(filePath.data());
        struct dirent *files;
        if(fileDir != NULL)
        {
            while((files = readdir(fileDir)) != NULL)
            {
                char* nameptr = files->d_name;
                if (!nameptr)
                {
                    ERROR("File name was nullptr, skipping...");
                    continue;
                }
                std::string fileName(nameptr);
                if (fileName == "." || fileName == "..") continue;
                std::string foundExtension = GetExtension(fileName);
                if(foundExtension.find(extension) != std::string::npos)
                {
                    out.push_back(fileName);
                    foundTheExtension = true; 
                }
            }
            closedir(fileDir);
        }
        return foundTheExtension;
    }  
}