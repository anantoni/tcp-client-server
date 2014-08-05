#ifndef CONNECTION_H
#define CONNECTION_H
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <netdb.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

class Connection {
    std::string server_ip;
    std::string dir_path;
    int port;
    int sock;
public:
    Connection(std::string, int, std::string);
};

#endif
