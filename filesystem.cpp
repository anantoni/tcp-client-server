#include "filesystem.hpp"

void Filesystem::createHierarchy(std::string file_path) {
    std::string delimiter = "/";
    size_t pos = 0;
    std::string token;
    std::string path_to_dir;
    unsigned found = file_path.find_last_of("/\\");

    file_path = file_path.substr(0,found);
    if (file_path[0] == '.') {
        while ((pos = file_path.find(delimiter)) != std::string::npos) {
            token = file_path.substr(0, pos);
            path_to_dir.append(token+'/');
            std::cout << token << std::endl;
            file_path.erase(0, pos + delimiter.length());

            struct stat s;
            int err = stat(file_path, &s);
            if(-1 == err) {
                if(ENOENT == errno) {
                    if((mkdir(path_to_dir,00777))==-1) {
                        perror("mkdir");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    perror("stat");
                    exit(EXIT_FAILURE);
                }
            } else {
                if(S_ISDIR(s.st_mode)) {
                    /* it's a dir */
                } else {
                    /* exists but is no dir */
                }
            }'}}
        }
    }

    std::cout << file_path << std::endl;
}
