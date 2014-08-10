#ifndef WORKER_HPP
#define WORKER_HPP

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

class Worker {
private:
    int id;
public:
    ~Worker();
    static void *dispatch(void*);
    void handleTasks();
    void run();
    static pthread_t consumer;
};

#endif
