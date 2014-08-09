#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <signal.h>
#include "../server/worker.hpp"
#include "task_queue.hpp"

extern volatile sig_atomic_t flag;
extern Worker* workers;
extern int sock;
extern int pool_size;
extern TaskQueue* task_queue;

void termination_handler(int signum);
void perror2(std::string s, int e);

#endif
