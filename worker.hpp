#ifndef WORKER_H
#define WORKER_H

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
