// Only commenting lines which are not selfexplanatory in their mnemonic.

#include "chunk_server.h"

/*
 * 1. Handling arguments passed by admin for server execution, making possibility for creating multiple ports for server
 * 2. Parsing arguments to port numbers
 * 3. Creating a pair of sockets, listening both to udp and tcp on parsed ports
 * 4. Accept new clients in an infinite loop
 * 5. Accept and add the new TCP client fd to the list of connected TCP clients; then spawn a thread for this client
 * 6. Create and operate with UDP connection
 */
int main(int argc, char* argv[]) {
	// 1. Handling arguments passed by admin for server execution, making possibility for creating multiple ports for server
	if (argc < 3) {
		cerr << "Error parsing arguments, should be like 'server tcp_port udp_port'" << endl;
		return -1;
	}
	
	// 2. Parsing arguments to port numbers
	int tcp_port = atoi(argv[1]);
	int udp_port = atoi(argv[2]);

	// 3. Creating a pair of sockets, listening both to udp and tcp on parsed ports
	chunk_server server;
	if (server.set_socket(SOCK_STREAM, tcp_port) < 0 ||	// set_socket creates sockets, binds them with internal ip and provided port, then starts listening on them, if its TCP
		server.set_socket(SOCK_DGRAM, udp_port) < 0) {	// if UPD - just binds them
		return -1;
	}

    // 4. Accept new clients in an infinite loop
    while (true) {
		
		// 5. Accept and add the new TCP client fd to the list of connected TCP clients; then spawn a thread for this client
		int tcp_client_socket_fd = server.accept_tcp_client();
		if (tcp_client_socket_fd < 0) {
			continue;	// if we fail on one accept we want to restart the loop
		}
		server.add_tcp_thread(tcp_client_socket_fd);
		// 6. Create and operate with UDP connection
		server.add_udp_thread();
	}
	
	delete &server;

    return 0;	// ASCII standart requirment
}
