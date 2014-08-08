#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include "task.hpp"

class TaskQueue;

extern TaskQueue* task_queue;

class TaskQueue {
private:
    pthread_mutex_t task_queue_mutex;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
    Task* task_queue;
    int pool_size;
    int count;
    int end;
    int start;
public:
    TaskQueue(int limit);
    void addTask(Task task);
    Task removeTask();
};

#endif
