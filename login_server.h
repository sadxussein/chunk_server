#ifndef LOGIN_SERVER_H
#define LOGIN_SERVER_H

#include "server.h"

class Login_server: public Server {
public:
    Login_server();
    void handle_tcp_client(int client_tcp_socket_fd) override;
    void handle_udp_client() override;
    ~Login_server();
protected:
private:
    bool userExists;
};

#endif // LOGIN_SERVER_H