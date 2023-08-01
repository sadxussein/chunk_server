#ifndef SERVER_H
#define SERVER_H

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

//extern const int MY_CONSTANT;
//extern const int BUFFER_SIZE;
const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1500;

// TODO: need to think about threads

class Server {
public:
	int get_tcp_socket();
	int get_udp_socket();
	int set_socket(int type, int port);							// create socket file descriptor for server of 'type' on 'port'
	int accept_tcp_client();									// accepting TCP connection from client and storing it
	virtual void add_tcp_thread(int client_tcp_socket_fd) = 0; 	// TODO: creating separate thread for client
	virtual void add_udp_thread() {                             // TODO: creating separate thread for client
        std::cout << "No UDP implemented" << std::endl;
    }
    void remove_user(int client_tcp_socket_fd);
protected:
	struct sockaddr_in server_address;					// server socket address structure
	int server_tcp_socket_fd, server_udp_socket_fd;		// main filedescriptors, which fork into client connections fd's
	std::map<int, sockaddr_in*> clients_tcp_addresses;	// here we store TCP fd's with client hints
	std::mutex clients_mutex;							// thread data separation
private:
};

#endif // SERVER_H