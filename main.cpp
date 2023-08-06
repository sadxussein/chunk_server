// Only commenting lines which are not selfexplanatory in their mnemonic.

#include "chunk_server.h"
#include "login_server.h"
#include "game_server.h"

/*
 * TODO: Linux pthread_t required
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
    Game_server game_server;
    if (game_server.set_socket(SOCK_STREAM, game_tcp_port) < 0) {
        return -1;
    }

	Chunk_server chunk_server;
	if (chunk_server.set_socket(SOCK_STREAM, chunk_tcp_port) < 0 ||	// set_socket creates sockets, binds them with internal ip and provided port, then starts listening on them, if its TCP
		chunk_server.set_socket(SOCK_DGRAM, chunk_udp_port) < 0) {	// if UPD - just binds them
		return -1;
	}

    std::map<Client_fd_pool, Client> client_pool;

    // 4. Accept new clients in an infinite loop TODO: make login threads, at this point we can have only 1 login attempt
    while (true) {
        // 5. Accept login client
        Client_fd_pool client_fd_pool;
        if (login_server.accept_tcp_client(client_pool, client_fd_pool) < 0) {
            std::cerr << "Login socket error" << std::endl;
            continue;   // if we fail on one accept we want to restart the loop
        }
        // TODO: remove DEBUG
        for (const auto& c: client_pool) {
            std::cout << "client login tcp fd: " << c.first.login_tcp_fd << ' ' << c.second.fdPool.login_tcp_fd << std::endl;
            std::cout << "client chunk fd: " << c.first.chunk_tcp_fd << ' ' << c.second.fdPool.chunk_tcp_fd << std::endl;
            std::cout << "client game fd: " << c.first.game_tcp_fd << ' ' << c.second.fdPool.game_tcp_fd << std::endl;
            std::cout << "IP/port: " << inet_ntoa(c.second.addr.sin_addr) << ' ' << c.second.addr.sin_port << std::endl;
        }
        // DEBUG end
        login_server.add_tcp_thread(client_fd_pool.login_tcp_fd);
        std::cout << "Created login thread" << std::endl;

        if (game_server.accept_tcp_client(client_pool, client_fd_pool) < 0) {
            std::cerr << "Game socket error" << std::endl;
            continue;       // TODO: ???
        }
        std::cout << "Accepted game client" << std::endl;
        game_server.add_tcp_thread(client_fd_pool.game_tcp_fd);
        std::cout << "Created game thread" << std::endl;

		// 5. Accept and add the new TCP client fd to the list of connected TCP clients; then spawn a thread for this client
		if (chunk_server.accept_tcp_client(client_pool, client_fd_pool) < 0) {
            std::cerr << "Chunk socket error" << std::endl;
			continue;	// if we fail on one accept we want to restart the loop
		}
        std::cout << "Accepted chunk client" << std::endl;
		chunk_server.add_tcp_thread(client_fd_pool.chunk_tcp_fd);
		// 6. Create and operate with UDP connection
		chunk_server.add_udp_thread();
        std::cout << "Created chunk threads" << std::endl;
	}
	
	delete &chunk_server;
    delete &login_server;

    return 0;	// ASCII standard requirement
}
