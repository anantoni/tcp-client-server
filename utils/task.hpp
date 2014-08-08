#ifndef TASK_HPP
#define TASK_HPP

#include <iostream>
#include <pthread.h>
#include "connection.hpp"

class Task {
private:
    const char* file_name;
    int sock;
    pthread_mutex_t *socket_lock;
    Connection *conn;
public:
    Task(){};
    ~Task();
    Task(const Task& task);
    Task(std::string file_name, int sock, pthread_mutex_t *socket_lock, Connection* conn);
    Task& operator=(const Task& task);
    const char* getFileName() const;
    int getSocket() const;
    Connection* getConnection();
    pthread_mutex_t* getSocketMutex();
    void clearResources();
};

#endif
