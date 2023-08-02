// Only commenting lines which are not selfexplanatory in their mnemonic.

#include "chunk_server.h"
#include "login_server.h"
//#include "game_server.h"

/*
 * 1. Handling arguments passed by admin for server execution, making possibility for creating multiple ports for server
 * 2. Parsing arguments to port numbers
 * 3. Creating a bunch of servers and listening on parsed tcp/udp ports
 * 4. Accept new clients in an infinite loop
 * 5. Accept and add the new TCP client fd to the list of connected TCP clients; then spawn a thread for this client
 * 6. Create and operate with UDP connection
 */
int main(int argc, char* argv[]) {
	// 1. Handling arguments passed by admin for chunk_server execution, making possibility for creating multiple ports for chunk_server
	if (argc < 5) {
        std::cerr << "Error parsing arguments, should be like 'server login_tcp_port game_tcp_port chunk_tcp_port chunk_udp_port'" << std::endl;
		return -1;
	}
	
	// 2. Parsing arguments to port numbers
	int login_tcp_port = atoi(argv[1]);
    int game_tcp_port  = atoi(argv[2]);
	int chunk_tcp_port = atoi(argv[3]);
    int chunk_udp_port = atoi(argv[4]);

    // 3. Creating a bunch of servers and listening on parsed tcp/udp ports
    Login_server login_server;
    if (login_server.set_socket(SOCK_STREAM, login_tcp_port) < 0) {
        return -1;
    }

    // TODO: finish game server class
//    Game_server game_server;
//    if (game_server.set_socket(SOCK_STREAM, game_tcp_port) < 0) {
//        return -1;
//    }

	Chunk_server chunk_server;
	if (chunk_server.set_socket(SOCK_STREAM, chunk_tcp_port) < 0 ||	// set_socket creates sockets, binds them with internal ip and provided port, then starts listening on them, if its TCP
		chunk_server.set_socket(SOCK_DGRAM, chunk_udp_port) < 0) {	// if UPD - just binds them
		return -1;
	}

    // 4. Accept new clients in an infinite loop
    while (true) {
        // 5. Accept login client
		ssize_t login_tcp_client_socket_fd = login_server.accept_tcp_client();
        if (login_tcp_client_socket_fd < 0) {
            continue;   // if we fail on one accept we want to restart the loop
        }
        login_server.add_tcp_thread(login_tcp_client_socket_fd);

		// 5. Accept and add the new TCP client fd to the list of connected TCP clients; then spawn a thread for this client
		int tcp_client_socket_fd = chunk_server.accept_tcp_client();
		if (tcp_client_socket_fd < 0) {
			continue;	// if we fail on one accept we want to restart the loop
		}
		chunk_server.add_tcp_thread(tcp_client_socket_fd);
		// 6. Create and operate with UDP connection
		chunk_server.add_udp_thread();
	}
	
	delete &chunk_server;
    delete &login_server;

    return 0;	// ASCII standard requirement
}
