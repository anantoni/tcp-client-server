#include <iostream>
#include <cstdlib>
#include "client_options.hpp"
#include "connection.hpp"

int main(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Invalid number of arguments." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> argsVector = parse_options(argc, argv);
    std::string server_ip =  argsVector.at(0);
    std::string dir_path = argsVector.at(2);
    int port = atoi(argsVector.at(1).c_str());
    int sock;

    struct sockaddr_in server ;
    struct sockaddr *serverptr = (struct sockaddr *) &server ;
    struct hostent *rem;
    char* file_name;

    /* Create socket */
    if ((sock = socket(PF_INET , SOCK_STREAM , 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Find server address */
    if ((rem = gethostbyname(server_ip.c_str())) == NULL) {
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET; /* Internet domain */
    memcpy(& server.sin_addr, rem -> h_addr, rem -> h_length);
    server.sin_port = htons(port); /* Server port */

    /* Initiate connection */
    if (connect(sock, serverptr, sizeof(server) ) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    std::cout << "Connecting to " << server_ip << " port " <<  port << std::endl;

    Connection conn(sock, server_ip, port, dir_path);
    conn.requestDir();

    while (1) {
        int file_size, bytes_to_read;
        Connection::readAll(sock, &bytes_to_read, sizeof(int));
        std::cout << "waiting to read: " << bytes_to_read << " bytes" << std::endl;
        if ((file_name = (char*)malloc(bytes_to_read)) == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        Connection::readAll(sock, file_name, bytes_to_read);
        std::cout << "file name: " << file_name << std::endl;
        Connection::readAll(sock, &file_size, sizeof(int));
        std::cout << "file size: " << file_size << std::endl;

        int n = 0;
        int pagesize = sysconf(_SC_PAGESIZE);
        char buf[pagesize];
        while (file_size > 0) {
            if ((n = Connection::readAll(sock, buf,pagesize )) < 0) {
                perror("readAll");
                exit(EXIT_FAILURE);
            }
            file_size -= n;
        }
    }

    return 0;
}
