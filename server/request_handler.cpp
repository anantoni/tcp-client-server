#include "request_handler.hpp"
#include "../utils/task_queue.hpp"
#include "../utils/connection.hpp"
#include "../utils/signal_handler.hpp"

RequestHandler::RequestHandler(Connection &conn) {
    this->conn = conn;
    if ((socket_lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t))) == NULL ) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(socket_lock, NULL) != 0) {
        perror("mutex init failed");
        exit(EXIT_FAILURE);
    }
}

/*
 * Recursively find all files in subdirectories of dir_path
 */
void RequestHandler::exploreHierarchy(std::string dir_path) {
    unsigned char isFile = 0x8;
    unsigned char isDir = 0x4;
    DIR *ent;
    struct dirent *dir_entry;

    if (dir_path[0] != '/' && dir_path[0] != '.')
        dir_path = dir_path.insert(0, "./");

    if (dir_path.back() != '/')
        dir_path.push_back('/');

    if ((ent = opendir(dir_path.c_str())) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while((dir_entry=readdir(ent)) != NULL)  {

        // If file: add file path to vector addToQueue
        if (dir_entry->d_type == isFile) {
            Task* task = new Task(dir_path + dir_entry->d_name, conn.getSocket(), socket_lock, &conn);
            pendingTasks.push_back(task);
        }

        // If dir: avoid . and .. and recursively explore subdirectories
        else if (dir_entry->d_type == isDir) {
            if (!strcmp(dir_entry->d_name, "..") || !strcmp(dir_entry->d_name, "."))
                continue;
            exploreHierarchy(dir_path + dir_entry->d_name);
        }
    }
}

void RequestHandler::exploreHierarchy() {
    exploreHierarchy(conn.getDirPath());
    int file_number = pendingTasks.size();
    conn.setFileNumber(file_number);
    file_number = htonl(file_number);
    Connection::writeAll(conn.getSocket(), &file_number, sizeof(int));
}

void RequestHandler::run() {
    pthread_t producer;
    if (pthread_create(&producer, NULL, dispatch, (void *) this) != 0) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
    }
    if (pthread_detach(producer) != 0) {
        perror("Thread detach");
        exit(EXIT_FAILURE);
    }
}

void *RequestHandler::dispatch(void *arg) {
    RequestHandler *handler = (RequestHandler *) arg;
    handler->handleRequest();
    delete handler;
    return 0;
}

void RequestHandler::handleRequest() {
    exploreHierarchy();
    for (Task* task : pendingTasks)
        task_queue->addTask(task);
}
