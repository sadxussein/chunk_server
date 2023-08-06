#include "client_fd_pool.h"

/*
 * overloading < operator for std::map find() and insert() to work
 */
bool Client_fd_pool::operator<(const Client_fd_pool &other) const {
    return login_tcp_fd < other.login_tcp_fd;
}
