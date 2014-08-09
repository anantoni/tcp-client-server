#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include "task.hpp"

class TaskQueue;

class TaskQueue {
private:
    pthread_mutex_t task_queue_mutex;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
    Task** task_queue;
    int pool_size;
    int count;
    int end;
    int start;
public:
    TaskQueue(int limit);
    ~TaskQueue();
    void addTask(Task* task);
    void broadcastToTerminate();
    Task* removeTask();
};

#endif
