#include "client_fd_pool.h"

/*
 * overloading == operator for std::unordered_map search
 */
bool Client_fd_pool::operator==(const Client_fd_pool &other) const {
    return login_tcp_fd == other.login_tcp_fd &&
           game_tcp_fd == other.game_tcp_fd &&
           chunk_tcp_fd == other.chunk_tcp_fd;
}






