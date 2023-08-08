#ifndef LOGIN_SERVER_H
#define LOGIN_SERVER_H

#include <unordered_map>
#include "server.h"

class Login_server: public Server {
public:
    Login_server();
    int accept_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) override;
    void handle_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) override;
    void handle_udp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) override;
    ~Login_server();
protected:
private:
    bool userExists;
};

#endif // LOGIN_SERVER_H