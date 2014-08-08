#include "client_options.hpp"

std::vector<std::string> parse_options(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> parsedArgs;
    std::string flag = "";
    std::string server_ip = "";
    std::string dir_path;
    int port;

    for (std::string& s : args) {
        std::cout << s << std::endl;

        if (s.compare("-i") == 0)
            flag = "server_ip";
        else if (s.compare("-p") == 0)
            flag = "server_port";
        else if (s.compare("-d") == 0)
            flag = "dir_path";
        else {
            if (flag.compare("server_ip") == 0)
                server_ip = s;
            else if (flag.compare("server_port") == 0)
                std::istringstream(s) >> port;
            else if (flag.compare("dir_path") == 0)
                dir_path = s;
            else {
                std::cerr << "Invalid options" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    /******** Add arguments to vector ********/
    parsedArgs.push_back(server_ip);
    parsedArgs.push_back(std::to_string(port));
    parsedArgs.push_back(dir_path);

    return parsedArgs;
}
