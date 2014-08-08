#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <netdb.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pthread.h>

class Connection {
private:
    std::string server_ip;
    std::string dir_path;
    int port;
    int sock;
    int file_number;
    pthread_mutex_t file_number_lock;
public:
    Connection(){};
    Connection(int, std::string, int, std::string);
    Connection(int, int);
    Connection(const Connection& conn);
    void requestDir();
    void receiveDirRequest();
    static int writeAll(int fd, void* buf, size_t size);
    static int readAll(int fd, void* buf, size_t size);
    std::string getDirPath() const;
    std::string getServerIp() const;
    int getSocket() const;
    int getPort() const;
    pthread_mutex_t *getFileMutex();
    int checkTransferCompletion();
    void setFileNumber(int file_number);
};

#endif
