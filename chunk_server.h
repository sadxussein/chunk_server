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
	map<int, sockaddr*> clients_addresses;
	vector<int> clients;

	chunk_server();	// +
	int set_socket(int type, int port);	// +
	int get_tcp_socket();	// +
	int get_udp_socket();	// +
	void add_tcp_client(int client_tcp_socket, struct sockaddr* client_addr);	// ???
	void add_udp_client(int client_tcp_socket, struct sockaddr* client_addr);	// ???
	void handle_tcp_client(int socket);	// ???
	void handle_udp_client(int socket);	// ???
	void add_thread(int client_tcp_socket);
	~chunk_server();	// +

private:
	int tcp_socket, udp_socket;		
	vector<string> chat;	// temporary chat	
	mutex clients_mutex;
};