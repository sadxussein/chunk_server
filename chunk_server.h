#ifndef CHUNK_SERVER_H
#define CHUNK_SERVER_H

#include "server.h"

class Chunk_server : public Server{
public:
    int accept_tcp_client(std::map<Client_fd_pool, Client> &client_pool_ptr, Client_fd_pool &client_fd_pool) override;
	void handle_tcp_client(int client_tcp_socket_fd) override;
	void handle_udp_client() override;
	~Chunk_server();
protected:
private:
};

#endif // CHUNK_SERVER_H
