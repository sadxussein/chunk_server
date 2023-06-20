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
	map<int, sockaddr_in> clients_addresses;
	vector<int> clients;

	chunk_server();
	void set_socket(int type, int port);
	int get_tcp_socket();
	int get_udp_socket();
	void handle_tcp_client();
	void handle_udp_client();
	~chunk_server();	

private:
	int tcp_socket, udp_socket;		
	vector<string> chat;	// temporary chat	
	mutex clients_mutex;
};