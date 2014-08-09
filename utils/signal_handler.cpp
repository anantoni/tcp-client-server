#include "signal_handler.hpp"

void termination_handler(int signum) {
    if (signum == SIGINT) {
        flag = 1;
        task_queue->broadcastToTerminate();
        sleep(3);
        delete task_queue;
        close(sock);
        //delete[] workers;
        exit(EXIT_SUCCESS);
    }
}
