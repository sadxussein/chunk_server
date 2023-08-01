#ifndef CHUNK_SERVER_H
#define CHUNK_SERVER_H

#include "server.h"
#include "player.h"

class Chunk_server : public Server{
public:
	Chunk_server();
	void handle_tcp_client(int client_tcp_socket_fd);
	void handle_udp_client();
	/*
	 * add_udp_thread
	 */
	void add_udp_thread() override {
        std::thread t([this]() { handle_udp_client(); });	// pointer to function
		t.detach();										    // now function will operate detached from others
	}
	/*
	 * add_tcp_thread
	 */
	void add_tcp_thread(int client_tcp_socket_fd) override {
		// following lambda construct is required for calling non-static function
        std::thread t([this, client_tcp_socket_fd]() {		// object pointer and function argument
			handle_tcp_client(client_tcp_socket_fd);	// pointer to function
		});
		t.detach();	// now function will operate detached from others
	}
	~Chunk_server();
protected:
private:
};

#endif // CHUNK_SERVER_H