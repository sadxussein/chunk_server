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

    Game_server game_server;
    if (game_server.set_socket(SOCK_STREAM, game_tcp_port) < 0) {
        return -1;
    }

    // TODO: multiple chunks required
	Chunk_server chunk_server;
	if (chunk_server.set_socket(SOCK_STREAM, chunk_tcp_port) < 0 ||	// set_socket creates sockets, binds them with internal ip and provided port, then starts listening on them, if its TCP
		chunk_server.set_socket(SOCK_DGRAM, chunk_udp_port) < 0) {	// if UPD - just binds them
		return -1;
	}

    std::unordered_map<int, Client> client_pool;

    // 4. Accept new clients in an infinite loop TODO: make login threads, at this point we can have only 1 login attempt
    while (true) {
        // 5. Accept login client
        int client_login_tcp_fd = login_server.accept_tcp_client(client_pool, 0);
        if (client_login_tcp_fd < 0) {
            std::cerr << "Login socket error" << std::endl;
            continue;   // if we fail on one accept we want to restart the loop
        }
        std::cout << "Accepted login client" << std::endl;
        login_server.add_tcp_thread(client_pool, client_login_tcp_fd);
        std::cout << "Created login thread" << std::endl;
        for (const auto& c: client_pool) {
            std::cout << c.first << " client login tcp fd: " << c.second.fd_pool.login_tcp_fd << ' '
                      << "client chunk fd: " << c.second.fd_pool.chunk_tcp_fd << ' '
                      << "client game fd: " << c.second.fd_pool.game_tcp_fd << std::endl;
            std::cout << "IP/port: " << inet_ntoa(c.second.addr_pool.login_tcp_addr.sin_addr)
                      << ' ' << c.second.addr_pool.login_tcp_addr.sin_port << std::endl;
        }

        if (game_server.accept_tcp_client(client_pool, client_login_tcp_fd) < 0) {
            std::cerr << "Game socket error" << std::endl;
            continue;       // TODO: ???
        }
        std::cout << "Accepted game client" << std::endl;
        game_server.add_tcp_thread(client_pool, client_login_tcp_fd);
        std::cout << "Created game thread" << std::endl;
        for (const auto& c: client_pool) {
            std::cout << c.first << " client login tcp fd: " << c.second.fd_pool.login_tcp_fd << ' '
                      << "client chunk fd: " << c.second.fd_pool.chunk_tcp_fd << ' '
                      << "client game fd: " << c.second.fd_pool.game_tcp_fd << std::endl;
            std::cout << "IP/port: " << inet_ntoa(c.second.addr_pool.game_tcp_addr.sin_addr)
                      << ' ' << c.second.addr_pool.game_tcp_addr.sin_port << std::endl;
        }

		// 5. Accept and add the new TCP client fd to the list of connected TCP clients; then spawn a thread for this client
		if (chunk_server.accept_tcp_client(client_pool, client_login_tcp_fd) < 0) {
            std::cerr << "Chunk socket error" << std::endl;
			continue;	// if we fail on one accept we want to restart the loop
		}
        std::cout << "Accepted chunk client" << std::endl;
		chunk_server.add_tcp_thread(client_pool, client_login_tcp_fd);
		// 6. Create and operate with UDP connection
        chunk_server.add_udp_thread(client_pool, client_login_tcp_fd);
        std::cout << "Created chunk threads" << std::endl;
        for (const auto& c: client_pool) {
            std::cout << c.first << " client login tcp fd: " << c.second.fd_pool.login_tcp_fd << ' '
                      << "client chunk fd: " << c.second.fd_pool.chunk_tcp_fd << ' '
                      << "client game fd: " << c.second.fd_pool.game_tcp_fd << std::endl;
            std::cout << "IP/port: " << inet_ntoa(c.second.addr_pool.chunk_tcp_addr.sin_addr)
                      << ' ' << c.second.addr_pool.chunk_tcp_addr.sin_port << std::endl;
        }
	}
	
	delete &chunk_server;
    delete &login_server;

    return 0;	// ASCII standard requirement
}
