//
// Created by menegais1 on 28/07/2020.
//

#include "FileLoader.h"

std::string FileLoader::getPath(std::string path) {
#if CLION == 1
    return "../src/" + path;
#endif
#if CODEBLOCKS == 1
    return "./EngineRenderer/src/" + path;
#endif
    return "";
}

std::string FileLoader::getCurrentPath() {
    char buff[FILENAME_MAX]; //create string buffer to hold path
    GetCurrentDir(buff, FILENAME_MAX);
    return buff;
}
