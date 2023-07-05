#include "server.h"
#include "player.h"

using namespace std;

class chunk_server : public Server{
public:
	chunk_server();
	void handle_tcp_client(int client_tcp_socket_fd);
	void handle_udp_client();
	/*
	 * add_udp_thread
	 */
	void add_udp_thread() override {
		thread t([this]() { handle_udp_client(); });	// pointer to function
		t.detach();										// now function will operate detached from others
	}
	/*
	 * add_tcp_thread
	 */
	void add_tcp_thread(int client_tcp_socket_fd) override {
		// following lambda construct is required for calling non-static function
		thread t([this, client_tcp_socket_fd]() {		// object pointer and function argument
			handle_tcp_client(client_tcp_socket_fd);	// pointer to function
		});
		t.detach();	// now function will operate detached from others
	}
	~chunk_server();
protected:
	
private:
	
};