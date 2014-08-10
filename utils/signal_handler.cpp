#include "signal_handler.hpp"

void termination_handler(int signum) {
    if (signum == SIGINT) {
        flag = 1;
        task_queue->broadcastToTerminate();
        sleep(1);
        std::cout << "Deleting " << pool_size << " workers " << std::endl;
        for(int i=0; i<pool_size; i++)
            delete workers[i];
        delete[] workers;
        delete task_queue;
        close(sock);
        exit(EXIT_SUCCESS);
    }
}
