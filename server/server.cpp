#include <stdio.h>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* gethostbyaddr */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <iostream>
#include "../utils/connection.hpp"
#include "../utils/task_queue.hpp"
#include "../utils/signal_handler.hpp"
#include "request_handler.hpp"
#include "worker.hpp"
#include "server_options.hpp"

TaskQueue* task_queue;
Worker** workers;
volatile sig_atomic_t flag = 0;
int sock, pool_size;

int main(int argc, char** argv) {
    int port, newsock, queue_size;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *) &server;
    struct sockaddr *clientptr = (struct sockaddr *) &client;
    struct sigaction new_action, old_action;

    /* Set up the structure to specify the new action. */
    new_action.sa_handler = termination_handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction (SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction (SIGINT, &new_action, NULL);

    std::vector<int> argsVector = parse_options(argc, argv);
    port = argsVector.at(0);
    pool_size = argsVector.at(1);
    queue_size = argsVector.at(2);

    if ((task_queue = new TaskQueue(queue_size)) == NULL) {
        perror("new");
        exit(EXIT_FAILURE);
    }

    if ((workers = (Worker**)malloc(pool_size*sizeof(Worker*))) == NULL) {
        perror("malloc[]");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<pool_size; i++) {
        if ((workers[i] = new Worker) == NULL ) {
            perror("new");
            exit(EXIT_FAILURE);
        }
        workers[i]->run();
    }

    /* Create socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */

    int option=1;
    /* set socket options */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0) {
        perror("setsockopt error");
        exit(EXIT_FAILURE);
    }
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Listen for connections */
    if (listen(sock, 32) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening for connections to port " << port << std::endl;

    while (1) {
        /* accept connection */
        if ((newsock = accept(sock , clientptr , &clientlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        Connection conn(newsock, port);
        /* Find client’s address */
        printf("Accepted connection" );
        conn.receiveDirRequest();
        RequestHandler *request_handler = new RequestHandler(conn);
        request_handler->run();
    }
    return 0;
}
