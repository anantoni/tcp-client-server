#include <stdio.h>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* gethostbyaddr */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <iostream>
#include "server_options.h"

int main(int argc, char** argv) {
    int port, sock, newsock, queue_size, pool_size;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *) &server;
    struct sockaddr *clientptr = (struct sockaddr *) &client;
    struct hostent *rem;

    std::vector<int> argsVector = parse_options(argc, argv);
    port = argsVector.at(0);
    pool_size = argsVector.at(1);
    queue_size = argsVector.at(2);

    /* Create socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0) ) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */

    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Listen for connections */
    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening for connections to port " << port << std::endl;

    while (1) {
        /* accept connection */
        if ((newsock = accept ( sock , clientptr , & clientlen)) < 0) {
            perror( "accept");
            exit(EXIT_FAILURE);
        }

        /* Find client’s address */
        printf(" Accepted connection \n" );

        close ( newsock ); /* parent closes socket to client */

    }
    return 0;
}
