#ifndef SERVER_CLIENT_FD_POOL_H
#define SERVER_CLIENT_FD_POOL_H

#include <unordered_map>

struct Client_fd_pool {
    int login_tcp_fd;
    int game_tcp_fd;
    int chunk_tcp_fd;

    Client_fd_pool(): login_tcp_fd(-1), game_tcp_fd(-1), chunk_tcp_fd(-1) {}
//    Client_fd_pool() = default;
    bool operator==(const Client_fd_pool &other) const;
};

/*
 * hash template redefinition for struct to be the key for unordered_map
 * TODO: consider removal
 */
template<>
struct std::hash<Client_fd_pool> {
    std::size_t operator()(const Client_fd_pool& pool) const {
        std::size_t hash_login = std::hash<int>()(pool.login_tcp_fd);
        std::size_t hash_game = std::hash<int>()(pool.game_tcp_fd);
        std::size_t hase_chunk = std::hash<int>()(pool.chunk_tcp_fd);
        // some magic to get single hash-value for key
        return hash_login ^ (hash_game << 1) ^ (hase_chunk << 2);
    }
};

#endif //SERVER_CLIENT_FD_POOL_H
