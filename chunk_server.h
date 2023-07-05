#include "server.h"
#include "player.h"

using namespace std;

class chunk_server : public Server{
public:
	chunk_server();										// +
	void handle_tcp_client(int client_tcp_socket_fd);
	void handle_udp_client();
	/*
	 * add_udp_thread
	 */
	void add_udp_thread() override {
		thread t(handle_udp_client,	// pointer to function
				 this);								// object pointer
		t.detach();									// now function will operate detached from others
	}
	/*
	 * add_tcp_thread
	 */
	void add_tcp_thread(int client_tcp_socket_fd) override {
		thread t(handle_tcp_client,	// pointer to function
					this, 							// object pointer
					client_tcp_socket_fd);			// function argument
		t.detach();									// now function will operate detached from others
	}
	~chunk_server();									// +
protected:
	
private:
	
};