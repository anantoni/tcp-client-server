#include "connection.hpp"

Connection::Connection(int sock, std::string ip, int port, std::string path) {
    this->sock = sock;
    this->server_ip = ip;
    this->port = port;
    this->dir_path = path;
}

Connection::Connection(int sock, int port) {
    this->sock = sock;
    this->port = port;
    this->server_ip = "127.0.0.1";
}

Connection::Connection(const Connection &conn) {
    sock = conn.getSocket();
    server_ip = conn.getServerIp();
    port = conn.getPort();
    dir_path = conn.getDirPath();

}

void Connection::requestDir() {
    char *buf;
    int msg_size = dir_path.length()+1;

    if (writeAll(this->sock, &msg_size, sizeof(int)) == -1) {
        perror("send dir path length");
        exit(EXIT_FAILURE);
    }
    if ((buf = (char*)malloc((dir_path.length()+1)*sizeof(char))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(buf, dir_path.c_str(), dir_path.length()+1);
    if (writeAll(this->sock, buf, dir_path.length()+1) == 1) {
        perror("send dir path");
        exit(EXIT_FAILURE);
    }
}

void Connection::receiveDirRequest() {
    char *buf;
    int msg_size;

    if (readAll(this->sock, &msg_size, sizeof(int)) == -1) {
        perror("receive dir path");
        exit(EXIT_FAILURE);
    }

    if ((buf = (char*)malloc(msg_size*sizeof(char))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if ((readAll(this->sock, buf, msg_size)) == -1) {
        perror("receive dir path");
        exit(EXIT_FAILURE);
    }
    std::cout << buf << std::endl;
    dir_path = buf;
}

int Connection::writeAll(int fd, void *buf, size_t size) {
    int sent, n;
    for(sent = 0; (unsigned int)sent < size; sent+=n) {
        if ((n = write(fd, (char*)buf+sent, size-sent)) == -1) {
            return -1;
        }
    }
    return sent;
}

int Connection::readAll(int fd, void *buf, size_t size) {
    int received, n;
    for (received = 0; (unsigned int)received < size; received+=n) {
        if ((n = read(fd, (char*)buf+received, size-received)) == -1) {
            return -1;
        }
    }
    return received;
}

std::string Connection::getDirPath() const {
    return dir_path;
}

std::string Connection::getServerIp() const {
    return server_ip;
}

int Connection::getPort() const {
    return port;
}

int Connection::getSocket() const {
    return sock;
}
