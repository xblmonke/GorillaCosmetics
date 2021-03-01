#include "Utils/FileUtils.hpp"
#include <dirent.h>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include "beatsaber-hook/shared/utils/utils.h"
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
    void mkdir(std::string dir) 
    { 
        makeFolder(dir); 
    }

    void makeFolder(std::string directory)
    {
        if (!direxists(directory.c_str()))
        {
            int makePath = mkpath(directory.data());
            if (makePath == -1)
            {
                ERROR("Failed to make path %s", directory.c_str());
            }
        }
    }
}