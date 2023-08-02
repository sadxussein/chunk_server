#ifndef LOGIN_SERVER_H
#define LOGIN_SERVER_H

#include "server.h"

// Temporary user name and sha256 hashed qwerty123 password
const std::string USERNAME = "player";
const std::string USERPASS = "daaad6e5604e8e17bd9f108d91e26afe6281dac8fda0091040a7a6d7bd9b43b5";

class Login_server: public Server {
public:
    Login_server();
    void handle_tcp_client(int client_tcp_socket_fd) override;
    void handle_udp_client() override;
//    void add_tcp_thread(int client_tcp_socket_fd) override;
    ~Login_server();
protected:
private:
};

#endif // LOGIN_SERVER_H