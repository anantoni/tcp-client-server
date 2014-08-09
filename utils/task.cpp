#include <cstring>
#include <cstdlib>
#include "task.hpp"

Task::Task(std::string file_name, int sock, pthread_mutex_t *socket_lock, Connection* conn) {
    this->file_name = strdup(file_name.c_str());
    this->sock = sock;
    this->socket_lock = socket_lock;
    this->conn = conn;
}

Task::Task(const Task& task) {
    this->file_name = strdup(task.file_name);
    this->sock = task.sock;
    this->socket_lock = task.socket_lock;
    this->conn = task.conn;
}

Task& Task::operator=(const Task& task) {
    this->file_name = strdup(task.file_name);
    this->sock = task.sock;
    this->socket_lock = task.socket_lock;
    this->conn = task.conn;
    return *this;
}

const char* Task::getFileName() const {
    return file_name;
}

int Task::getSocket() const {
    return sock;
}

Connection* Task::getConnection() {
    return conn;
}

pthread_mutex_t* Task::getSocketMutex() {
    return socket_lock;
}

void Task::clearResources() {
    pthread_mutex_destroy(socket_lock);
    close(sock);
}

Task::~Task() {
    free((void*)file_name);
}
