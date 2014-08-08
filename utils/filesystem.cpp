#include "filesystem.hpp"

void Filesystem::createHierarchy(std::string file_path) {
    std::string delimiter = "/";
    size_t pos = 0;
    std::string token, path_to_dir;
    unsigned found = file_path.find_last_of("/\\");
    mode_t process_mask = umask(0);
    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

    file_path = file_path.substr(0, found+1);
    std::cout << "Processing path: " << file_path << std::endl;
    if (file_path[0] == '.') {
        while ((pos = file_path.find(delimiter)) != std::string::npos) {
            token = file_path.substr(0, pos);
            path_to_dir.append(token+'/');
            std::cout << "Token: " << token << std::endl;
            file_path.erase(0, pos + delimiter.length());
            struct stat s;
            int err = stat(path_to_dir.c_str(), &s);
            std::cout << "Found dir: " << path_to_dir << std::endl;

            if (err == -1) {
                if (errno == ENOENT) {
                    if ((mkdir(path_to_dir.c_str(), mode)) == -1)
                        continue;
                }
                else {
                    perror("stat");
                    exit(EXIT_FAILURE);
                }
            }
            else {
                if (S_ISDIR(s.st_mode))
                    continue;
                else
                    if ((mkdir(path_to_dir.c_str(), mode)) == -1)
                        continue;
            }
        }
    }
    umask(process_mask);
    std::cout << file_path << std::endl;
}
