#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "server.h"

class Game_server: public Server {
public:
    int accept_tcp_client(std::map<Client_fd_pool, Client> &client_pool_ptr, Client_fd_pool &client_fd_pool) override;
	void handle_tcp_client(int client_tcp_socket_fd) override;
	void handle_udp_client() override;
    ~Game_server();
protected:
	
private:
	
};

#endif // GAME_SERVER_H