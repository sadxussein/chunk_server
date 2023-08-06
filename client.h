#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "character.h"
#include "client_fd_pool.h"

struct Client {
    struct sockaddr_in addr{};                      // client address
    socklen_t addr_size = sizeof(addr);             //
    std::vector<Character> characters;              // client's game characters
    Client_fd_pool fdPool;                          // replacement for tcp_socket_fd
};

#endif //SERVER_CLIENT_H
