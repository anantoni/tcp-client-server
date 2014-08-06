#include "connection.h"

Connection::Connection(int sock, std::string server_ip, int port, std::string dir_path) {
    this->sock = sock;
    this->server_ip = server_ip;
    this->port = port;
    this->dir_path = dir_path;
}

Connection::Connection(int sock, int port) {
    this->sock = sock;
    this->port = port;
}

void Connection::requestDir() {
    char *buf;
    int msg_size = dir_path.length()+1;

    if (writeAll(&msg_size, sizeof(int)) == -1) {
        perror("send dir path length");
        exit(EXIT_FAILURE);
    }
    if ((buf = (char*)malloc((dir_path.length()+1)*sizeof(char))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(buf, dir_path.c_str(), dir_path.length()+1);
    if (writeAll(buf, dir_path.length()+1) == 1) {
        perror("send dir path");
        exit(EXIT_FAILURE);
    }
}

void Connection::receiveDirRequest() {
    char *buf;
    int msg_size;

    if (readAll(&msg_size, sizeof(int)) == -1) {
        perror("receive dir path");
        exit(EXIT_FAILURE);
    }

    if ((buf = (char*)malloc(msg_size*sizeof(char))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if ((readAll(buf, msg_size)) == -1) {
        perror("receive dir path");
        exit(EXIT_FAILURE);
    }
    std::cout << buf << std::endl;
}

int Connection::writeAll(void *buf, size_t size) {
    int sent, n;
    for(sent = 0; (unsigned int)sent < size; sent+=n) {
        if ((n = write(this->sock, (char*)buf+sent, size-sent)) == -1) {
            return -1;
        }
    }
    return sent;
}

int Connection::readAll(void *buf, size_t size) {
    int received, n;
    for (received = 0; (unsigned int)received < size; received+=n) {
        if ((n = read(this->sock, (char*)buf+received, size-received)) == -1) {
            return -1;
        }
    }
    return received;
}
