#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "client_options.hpp"
#include "../utils/filesystem.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

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

    int file_number;
    Connection::readAll(sock, &file_number, sizeof(int));
    std::cout << "Files to receive: " << file_number << std::endl;
    while (file_number > 0) {
        int file_size, bytes_to_read;

        /* read filename len */
        Connection::readAll(sock, &bytes_to_read, sizeof(int));
        std::cout << "waiting to read: " << bytes_to_read << " bytes" << std::endl;

        if ((file_name = (char*)malloc(bytes_to_read)) == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        /* read filename */
        Connection::readAll(sock, file_name, bytes_to_read);
        std::cout << "file name: " << file_name << std::endl;

        /* create the necessary directories */
        Filesystem::createHierarchy(strdup(file_name));

        /* read file size (int) */
        Connection::readAll(sock, &file_size, sizeof(int));
        std::cout << "file size: " << file_size << std::endl;

        int n = 0, pagesize = sysconf(_SC_PAGESIZE), fd;
        char* buf;
        if ((buf = (char*)malloc(pagesize)) == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        std::cout << "creating file name: " << file_name << std::endl;
        if ((fd = open(file_name, O_CREAT|O_WRONLY|O_TRUNC, mode)) < 0) {
            perror("open file\n");
            exit(EXIT_FAILURE);
        }

        /* read file by pagesize */
        while (file_size > 0) {
            std::cout << "Entered " << file_name << " loop" << std::endl;
            int bytes_to_read;
            if (file_size < pagesize)
                bytes_to_read = file_size;
            else
                bytes_to_read = pagesize;
            if ((n = Connection::readAll(sock, buf, bytes_to_read)) < 0) {
                perror("readAll");
                exit(EXIT_FAILURE);
            }
            file_size -= n;
            std::cout << "Writing " << n << " bytes to file " << std::endl;
            if (n > 0) {
                if (write(fd, buf, n) < 0) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }
        }
        free(buf);
        close(fd);
        file_number--;
    }
    close(sock);
    return 0;
}
