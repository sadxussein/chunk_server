#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "character.h"

struct Client {
    struct sockaddr_in addr;                        // client address
    socklen_t addr_size = sizeof(addr);             //
    int tcp_socket_fd;                              // client tcp socket file descriptor
    std::vector<Character> c;                       // client's game characters
};

#endif //SERVER_CLIENT_H
