#include "task.hpp"

Task::Task(std::string file_name, int sock, pthread_mutex_t* socket_lock) {
    this->file_name = file_name;
    this->sock = sock;
    this->socket_lock = socket_lock;
}

Task::Task(const Task& task) {
    this->file_name = task.getFileName();
    this->sock = task.getSocket();
    this->socket_lock = task.getMutex();
}

std::string Task::getFileName() const {
    return file_name;
}

int Task::getSocket() const {
    return sock;
}

pthread_mutex_t* Task::getMutex() const {
    return socket_lock;
}
