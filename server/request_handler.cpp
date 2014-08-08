#include "request_handler.hpp"
#include "../utils/task_queue.hpp"

RequestHandler::RequestHandler(Connection &conn) {
    this->conn = conn;
    socket_lock = PTHREAD_MUTEX_INITIALIZER ;
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

    std::cout << dir_path << std::endl;
    if ((ent = opendir(dir_path.c_str())) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while((dir_entry=readdir(ent)) != NULL)  {

        // If file: add file path to vector addToQueue
        if (dir_entry->d_type == isFile) {
            Task task(dir_path + dir_entry->d_name, conn.getSocket(), &socket_lock);
            std::cout <<"Adding file : " << dir_path + dir_entry->d_name << std::endl;
            pendingTasks.push_back(task);
        }

        // If dir: avoid . and .. and recursively explore subdirectories
        else if (dir_entry->d_type == isDir) {
            std::cout << "Found a directory: " << dir_path + dir_entry->d_name << std::endl;
            if (!strcmp(dir_entry->d_name, "..") || !strcmp(dir_entry->d_name, "."))
                continue;
            exploreHierarchy(dir_path + dir_entry->d_name);
        }
    }
}

void RequestHandler::exploreHierarchy() {
    exploreHierarchy(conn.getDirPath());
}

void RequestHandler::run() {
    pthread_t producer;

    if (pthread_create(&producer, NULL, dispatch, (void *) this) != 0) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
    }
}

void *RequestHandler::dispatch(void *arg) {
    RequestHandler *handler = (RequestHandler *) arg;
    handler->handleRequest();
    // delete handler;
    return 0;
}

void RequestHandler::handleRequest() {
    exploreHierarchy();
    for (Task task : pendingTasks) {
        //std::cout << "Adding " << task.getFileName() << " to queue." << std::endl;
        task_queue->addTask(task);
    }
}
