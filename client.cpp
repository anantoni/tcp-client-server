#include <iostream>
#include <cstdlib>
#include "client_options.h"
#include "connection.h"

int main(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Invalid number of arguments." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> argsVector = parse_options(argc, argv);
    std::string server_ip =  argsVector.at(0);
    int port = atoi(argsVector.at(1).c_str());
    std::string dir_path = argsVector.at(2);

    Connection conn(server_ip, port, dir_path);
    return 0;
}
