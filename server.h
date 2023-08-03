#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <map>
#include <mutex>	    // mutual exclusion; provides access to protected and shared resources
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <functional>
#include <unistd.h>	    // posix syscalls
#include <pqxx/pqxx>    // postgres library
#include <sys/types.h>	// unix types for syscalls
#include <sys/socket.h>	// unix sockets
#include <netinet/in.h>	// unix types for network

#include "client.h"

// TODO: define?
const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1500;

class Server {
public:
	int get_tcp_socket() const;
	int get_udp_socket() const;
	int set_socket(int type, int port);				// create socket file descriptor for server of 'type' on 'port'
	int accept_tcp_client();						// accepting TCP connection from client and storing it
	void add_tcp_thread(int client_tcp_socket_fd);  // creating separate thread for client
	void add_udp_thread();
    void remove_client(int client_tcp_socket_fd);   // remove client from map
    int postgres_connect();
    void postgres_disconnect();
    virtual void handle_tcp_client(int client_tcp_socket_fd) = 0;
    virtual void handle_udp_client() = 0;
protected:
	struct sockaddr_in server_address;					// server socket address structure
	int server_tcp_socket_fd, server_udp_socket_fd;	    // main filedescriptors, which fork into client connections fd's
    pqxx::connection* connect;                          // postgres connection handler
//	std::map<int, sockaddr_in*> clients_tcp_addresses;	// here we store TCP fd's with client hints
	std::mutex mtx;							            // thread data separation
    std::map<int, Client> clients;
private:
};

#endif // SERVER_H