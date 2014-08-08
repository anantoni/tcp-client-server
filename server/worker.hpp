#ifndef WORKER_HPP
#define WORKER_HPP

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

class Worker {
public:
    static void *dispatch(void*);
    void handleTasks();
    void run();
};

#endif
