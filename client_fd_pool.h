#ifndef SERVER_CLIENT_FD_POOL_H
#define SERVER_CLIENT_FD_POOL_H

struct Client_fd_pool {
    int login_tcp_fd = -1;
    int game_tcp_fd  = -1;
    int chunk_tcp_fd = -1;

    bool operator<(const Client_fd_pool &other) const;
};

#endif //SERVER_CLIENT_FD_POOL_H
