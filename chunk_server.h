#ifndef CHUNK_SERVER_H
#define CHUNK_SERVER_H

#include <unordered_map>
#include "server.h"

class Chunk_server : public Server{
public:
    int accept_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) override;
	void handle_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) override;
	void handle_udp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) override;
	~Chunk_server();
protected:
private:
};

#endif // CHUNK_SERVER_H
