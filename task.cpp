#include "task.hpp"

Task::Task(std::string file_name, int sock, pthread_mutex_t* socket_lock) {
    this->file_name = file_name;
    this->sock = sock;
    this->socket_lock = socket_lock;
}

Task::Task(const Task& task) {
    std::cout << "Task copy constructor called" << std::endl;
    std::cout << task.getFileName() << std::endl;
    std::cout << task.getSocket() << std::endl;
    //std::cout << *(task.getMutex()) << std::endl;
    this->file_name = task.getFileName();
    std::cout << "passed string assign" << std::endl;
    this->sock = task.getSocket();
    std::cout << "passed int assign" << std::endl;
    this->socket_lock = task.getMutex();
    std::cout << "passed mutex pointer assign" << std::endl;
}

Task& Task::operator=(const Task& task) {
    std::cout << "Copying task string" << std::endl;
    this->file_name = task.file_name.c_str();// task.file_name;
    std::cout << "Copied task string" << std::endl;
    this->sock = task.sock;
    this->socket_lock = task.socket_lock;
    return *this;
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
