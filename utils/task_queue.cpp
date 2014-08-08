#include "task_queue.hpp"

TaskQueue::TaskQueue(int limit) {
    task_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_init(& cond_nonempty, 0) ;
    pthread_cond_init(& cond_nonfull, 0) ;

    if ((task_queue = (Task*)malloc(limit*sizeof(Task))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    this->pool_size = limit;
    count = 0;
    end = -1;
    start = 0;
}

void TaskQueue::addTask(Task task) {
    pthread_mutex_lock (&task_queue_mutex);
    while (count >= pool_size) {
        std::cout << " >> Found Buffer Full" << std::endl;
        pthread_cond_wait(&cond_nonfull , &task_queue_mutex);
    }

    end = (end + 1) % pool_size;
    task_queue[end] = task;
    count++;

    pthread_cond_signal(&cond_nonempty);
    pthread_mutex_unlock(&task_queue_mutex);
}

Task TaskQueue::removeTask() {
    Task task;

    pthread_mutex_lock(&task_queue_mutex);
    while (count <= 0) {
        printf (" >> Found Buffer Empty \n " ) ;
        pthread_cond_wait(&cond_nonempty, &task_queue_mutex);
    }

    task = task_queue[start];
    start = (start + 1) % pool_size ;
    count--;
    pthread_cond_signal(&cond_nonfull);
    pthread_mutex_unlock (&task_queue_mutex);

    return task;
}
