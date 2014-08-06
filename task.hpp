#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <pthread.h>

class Task {
private:
    std::string file_name;
    int sock;
    pthread_mutex_t *socket_lock;

public:
    Task(){};
    Task(const Task& task);
    Task(std::string file_name, int sock, pthread_mutex_t* socket_lock);
    std::string getFileName() const;
    int getSocket() const;
    pthread_mutex_t* getMutex() const;
};

#endif
