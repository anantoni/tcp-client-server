#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <iostream>
#include <pthread.h>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <pthread.h>
#include "task.hpp"
#include "connection.hpp"

class RequestHandler {
private:
    pthread_mutex_t socket_lock;
    Connection conn;
    std::vector<Task> pendingTasks;

public:
    static void *dispatch(void *);
    RequestHandler(Connection&);
    void exploreHierarchy(std::string);
    void exploreHierarchy();
    void handleRequest();
    void run();
};

#endif
