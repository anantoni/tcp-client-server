#include "worker.hpp"
#include "task_queue.hpp"
#include "connection.hpp"

void Worker::run() {
    pthread_t consumer;

    if (pthread_create(&consumer, NULL, dispatch, (void*) this) != 0) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
    }
}

void* Worker::dispatch(void *arg) {
    Worker *worker = (Worker *) arg;
    worker->handleTasks();
    return 0;
}

void Worker::handleTasks() {
    int fd, sock, pagesize = sysconf(_SC_PAGESIZE);
    pthread_mutex_t *socket_mutex;
    struct stat st;
    char buf[pagesize];

    while (1) {
        Task task = task_queue->removeTask();
        //std::cout << "Removing " << task.getFileName() << " from queue." << std::endl;

        if ((fd = open(task.getFileName(), O_RDONLY, S_IREAD)) <= 0) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        socket_mutex = task.getMutex();
        sock = task.getSocket();
        pthread_mutex_lock(socket_mutex);
        std::cout << "writing to client" << std::endl;

        int filename_len = strlen(task.getFileName()) + 1;

        Connection::writeAll(sock, &filename_len, sizeof(int));
        std::cout << "wrote filename len " << filename_len << std::endl;
        Connection::writeAll(sock, (void *) task.getFileName(), strlen(task.getFileName())+1);
        stat(task.getFileName(), &st);
        std::cout << st.st_size << std::endl;
        Connection::writeAll(sock, &st.st_size, sizeof(int));

        int read_from_file = 0;
        int n = 0;
        while (read_from_file < st.st_size) {
            if ((n = read(fd, buf, pagesize)) < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (n > 0)
                Connection::writeAll(sock, buf, pagesize);

            read_from_file += n;
            if (read_from_file == st.st_size)
                break;
        }
        std::cout << "wrote to client" << std::endl;
        pthread_mutex_unlock(socket_mutex);
        close(fd);

    }


}
