#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <iostream>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>

class Filesystem {
public:
    static void createHierarchy(std::string file_path);
};

#endif
