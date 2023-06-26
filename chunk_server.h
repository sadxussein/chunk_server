#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>	// mutual exclusion; provides access to protected and shared resources
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>	// posix syscalls
#include <sys/types.h>	// unix types for syscalls
#include <sys/socket.h>	// unix sockets
#include <netinet/in.h>	// unix types for network

using namespace std;

const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1500;

class chunk_server {
public:		
	map<int, sockaddr_in*> clients_tcp_addresses;		// +
	vector<sockaddr_in*> clients_udp_addresses;			// ???

	chunk_server();										// +
	int set_socket(int type, int port);					// +
	int get_tcp_socket();								// +
	int get_udp_socket();								// +
	int accept_tcp_client();							// +
	void handle_tcp_client(int client_tcp_socket_fd);	// +
	void handle_udp_client();							// ???
	void add_tcp_thread(int client_tcp_socket_fd); 		// +
	void add_udp_thread();								// ???
	~chunk_server();									// +

private:
	struct sockaddr_in server_address;					// server socket address structure
	int server_tcp_socket_fd, server_udp_socket_fd;		// main filedescriptors, which fork into client connections fd's
	vector<string> chat;								// ??? temporary chat
	mutex clients_mutex;								// thread data separation
};