#include "server.h"

/*
 * set_socket
 * 1. Create the server socket file descriptor (fd), it is main descriptor here
 * 2. Bind the server socket to a port
 * 3. Listen for incoming connections if TCP and save fd of our TCP connection
 * 4. Or just save our UDP connection fd
 */
int Server::set_socket(int type, int port) {
	// 1. Create the server socket file descriptor, it is main descriptor here
    int server_socket = socket(AF_INET,	// AF_INET = internet tpc/ip
								type, 	// SOCK_STREAM = tcp, SOCK_DGRAM = udp
								0);		// 0 = protocol will be selected by the system automatically
    if (server_socket < 0) {
        std::cerr << "Error creating server socket" << std::endl;
        return -1;
    }

    // 2. Bind the server socket to a port
    memset(&server_address, 		// filling structure
		   0, 						// with zeroes
		   sizeof(server_address));	// of size of said structure
    server_address.sin_family = AF_INET;			// still tcp/ip
    server_address.sin_addr.s_addr = INADDR_ANY;	// system will decide which address to use
    server_address.sin_port = htons(port);			// htons is important transformation of byte order from Little endian to Big endian
    if (bind(server_socket,							// binding socket
			 (struct sockaddr*) &server_address,	// to address structure
			 sizeof(server_address)) < 0) {			// of size of said structure
        std::cerr << "Error binding server socket to port" << std::endl;
        return -1;
    }
	
	// 3. Listen for incoming connections if TCP and save fd of our TCP connection
    if (type == SOCK_STREAM) {
        server_tcp_socket_fd = server_socket;			// save our server fd for internal use
        if (listen(server_socket, MAX_CLIENTS) < 0) {	// major difference between TCP and UDP is that TCP has to LISTEN to port for connections
            std::cerr << "Error listening for incoming connections" << std::endl;
            return -1;
        }
    }
	// 4. Or just save our UDP connection fd
    else if (type == SOCK_DGRAM) {
        server_udp_socket_fd = server_socket;
    }
	
	return 0;	// if everything OK return 0
}

/*
 * add_tcp_client
 * 1. Accepting pending TCP connections
 * 2. Storing info about client
 */
int Server::accept_tcp_client() {
	// 1. Accepting pending TCP
    Client client;
//	socklen_t addr_size;
//	struct sockaddr_in tcp_client_addr;		// TCP client address and other stuff
//	addr_size = sizeof(tcp_client_addr);	// its size
    client.tcp_socket_fd = accept(this->get_tcp_socket(),
                                  (struct sockaddr *) &client.addr,
                                  &client.addr_size);
//	int client_tcp_socket_fd = accept(this->get_tcp_socket(),	// we get client address info from accept; client_tcp_socket_fd stores file descriptor
//									  (struct sockaddr *) &tcp_client_addr,
//									  &addr_size);
	if (client.tcp_socket_fd < 0) {
		std::cerr << "Error accepting client connection" << std::endl;
		return -1;
	}

	// 2. Storing info about client
	mtx.lock();	    // locking other threads from accessing following data
	clients.insert(std::make_pair(client.tcp_socket_fd, client));	// store fd and client address info for server
	mtx.unlock();	// unlocking data
	
	return client.tcp_socket_fd;	// if everything is ok return client fd
}

/*
 * add_tcp_thread
 * takes overridden tcp handler function and creates separate thread with that
 */
void Server::add_tcp_thread(int client_tcp_socket_fd) {
    std::thread t(&Server::handle_tcp_client, this, client_tcp_socket_fd);
    t.detach();
}

/*
 * add_udp_thread
 * takes overridden udp handler function and creates separate thread with that
 */
void Server::add_udp_thread() {
    std::thread t(&Server::handle_udp_client, this);
    t.detach();
}

/*
 * get_tcp_socket
 */
int Server::get_tcp_socket() const {
    return server_tcp_socket_fd;
}

/*
 * get_udp_socket
 */
int Server::get_udp_socket() const {
    return server_udp_socket_fd;
}

/*
 * remove_user
 */
void Server::remove_client(int client_tcp_socket_fd) {
    // by locking we make sure that only one thread is able to
    mtx.lock();
    // execute next command
    // remove function does not remove elements, only moving
    clients.erase(client_tcp_socket_fd);
    // them to the end of the vector; returns iterator which points to first socket element. After that erase
    // removes elements starting from remove iterator to the end of the vector
    // unlocking data
    mtx.unlock();
    // close the client socket
    close(client_tcp_socket_fd);
}

/*
 * postgres_connect
 * just opening connection not closing it
 * between connect and disconnect functions subclass will override postgres handler
 * TODO: complete postgres connection
 */
int Server::postgres_connect() {
    try {
        connect = new pqxx::connection("dbname=mmorpg user=postgres password=qwerty123 hostaddr=127.0.0.1 port=5432");
        if (!connect->is_open()) {
            std::cerr << "Could not connect to database" << std::endl;
            return -1;
        }
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}

/*
 * postgres_disconnect
 * only closing connection
 */
void Server::postgres_disconnect() {
    delete connect;
}