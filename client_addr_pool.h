#ifndef CLIENT_ADDR_POOL_H
#define CLIENT_ADDR_POOL_H

#include <netinet/in.h>

struct Client_addr_pool {
    struct sockaddr_in login_tcp_addr{};                        // client login tcp address
    socklen_t login_tcp_addr_size = sizeof(login_tcp_addr);     //
    struct sockaddr_in game_tcp_addr{};                         // client game tcp address
    socklen_t game_tcp_addr_size = sizeof(game_tcp_addr);       //
    struct sockaddr_in chunk_tcp_addr{};                        // client chunk tcp address
    socklen_t chunk_tcp_addr_size = sizeof(chunk_tcp_addr);     //
    struct sockaddr_in chunk_udp_addr{};                        // client chunk udp address
    socklen_t chunk_udp_addr_size = sizeof(chunk_udp_addr);     //
};


#endif //CLIENT_ADDR_POOL_H
