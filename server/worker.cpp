#include "worker.hpp"
#include "../utils/connection.hpp"
#include "../utils/task_queue.hpp"
#include "../utils/signal_handler.hpp"

pthread_t Worker::consumer;

Worker::~Worker() {
    std::cout << "Deleting worker " << id << std::endl;
}

void Worker::run() {
    id = 1;
    if (pthread_create(&consumer, NULL, dispatch, (void*) this) != 0) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
    }
    if (pthread_detach(consumer) != 0) {
        perror("Thread detach");
        exit(EXIT_FAILURE);
    }
}

void* Worker::dispatch(void *arg) {
    Worker *worker = (Worker *) arg;
    worker->handleTasks();
    //delete worker;
    return 0;
}

void Worker::handleTasks() {
    int fd, sock, pagesize = sysconf(_SC_PAGESIZE);
    pthread_mutex_t *socket_mutex;
    struct stat st;
    char* buf;

    /* allocate buffer of pagesize bytes to read from file */
    if ((buf = (char*)malloc(pagesize)) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (1) {
        Task *task = task_queue->removeTask();
        /* if task* returned points to NULL terminate thread */
        if (task == NULL)
            break;

        /* open file to read its contents */
        if ((fd = open(task->getFileName(), O_RDONLY, S_IREAD)) <= 0) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        /* obtain the mutex and socket fd corresponding to the task */
        socket_mutex = task->getSocketMutex();
        sock = task->getSocket();

        /* enter critical section */
        pthread_mutex_lock(socket_mutex);
        std::cout << "Tread: " << pthread_self() << " writing to client" << std::endl;
        int filename_len = strlen(task->getFileName()) + 1;
        filename_len = htonl(filename_len);

        /* send filename length */
        Connection::writeAll(sock, &filename_len, sizeof(int));
        /* send filename */
        Connection::writeAll(sock, (void *) task->getFileName(), strlen(task->getFileName())+1);
        stat(task->getFileName(), &st);
        int file_size = htonl(st.st_size);
        /* send file size */
        Connection::writeAll(sock, &file_size, sizeof(int));

        int read_from_file = 0;
        int n = 0;
        while (read_from_file < st.st_size) {
            if ((n = read(fd, buf, pagesize)) < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            /* send n bytes to client */
            if (n > 0)
                Connection::writeAll(sock, buf, n);

            read_from_file += n;
            if (read_from_file == st.st_size)
                break;
        }
        /* check if transfer is complete in order to terminate communication */
        if (task->getConnection()->checkTransferCompletion() == 1)
            task->clearResources();
        pthread_mutex_unlock(socket_mutex);
        /* exited critical section */
        close(fd);
        free(task);
    }
    std::cout << "Thread: " << pthread_self() << " freeing buf" << std::endl;
    free(buf);
    std::cout << "Worker thread terminating" << std::endl;
}
