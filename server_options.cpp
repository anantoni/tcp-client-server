#include "server_options.hpp"
#include "connection.hpp"

std::vector<int> parse_options(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<int> parsedArgs;
    std::string flag = "";
    int port;
    int pool_size;
    int queue_size;

    for (std::string& s : args) {
        std::cout << s << std::endl;

        if (s.compare("-p") == 0)
            flag = "port";
        else if (s.compare("-s") == 0)
            flag = "pool_size";
        else if (s.compare("-q") == 0)
            flag = "queue_size";
        else {
            if (flag.compare("port") == 0)
                std::istringstream(s) >> port;
            else if (flag.compare("pool_size") == 0)
                std::istringstream(s) >> pool_size;
            else if (flag.compare("queue_size") == 0)
                std::istringstream(s) >> queue_size;
            else {
                std::cerr << "Invalid options" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    /******** Add arguments to vector ********/
    parsedArgs.push_back(port);
    parsedArgs.push_back(pool_size);
    parsedArgs.push_back(queue_size);

    return parsedArgs;
}
