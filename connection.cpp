#include "connection.h"

Connection::Connection(std::string server_ip, int port, std::string dir_path) {
    this->server_ip = server_ip;
    this->port = port;
    this->dir_path = dir_path;

    struct sockaddr_in server ;
    struct sockaddr *serverptr = (struct sockaddr *) &server ;
    struct hostent *rem;

    /* Create socket */
    if ((this->sock = socket(PF_INET , SOCK_STREAM , 0)) < 0) {
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
    if (connect(sock, serverptr , sizeof (server) ) < 0)
        perror("connect");

    std::cout << "Connecting to " << server_ip << "port " <<  port << std::endl;

}
