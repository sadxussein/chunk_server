#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "character.h"
#include "client_fd_pool.h"
#include "client_addr_pool.h"

// TODO: constructor Client()
struct Client {
    size_t id;                                  // unique client id from database
    Character character;                        // selected by client game character id
    Client_fd_pool fd_pool;                     // replacement for tcp_socket_fd
    Client_addr_pool addr_pool;                 // client's connections address/port pool
};

#endif //SERVER_CLIENT_H
