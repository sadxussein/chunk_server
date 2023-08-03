#ifndef CHUNK_SERVER_H
#define CHUNK_SERVER_H

#include "server.h"

class Chunk_server : public Server{
public:
	Chunk_server();
	void handle_tcp_client(int client_tcp_socket_fd) override;
	void handle_udp_client() override;
	~Chunk_server();
protected:
private:
};

#endif // CHUNK_SERVER_H
