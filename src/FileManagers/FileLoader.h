//
// Created by menegais1 on 28/07/2020.
//

#ifndef CARENGINE_FILELOADER_H
#define CARENGINE_FILELOADER_H


#include <string>

#define CLION 1
#define CODEBLOCKS 0


#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

class FileLoader {
public:
    static std::string getPath(std::string path);

    std::string getCurrentPath();
};


#endif //CARENGINE_FILELOADER_H
