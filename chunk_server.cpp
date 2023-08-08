#include <unordered_map>
#include "chunk_server.h"

/*
 * accept_tcp_client
 * 1. Accept pending TCP connection
 * 2. Storing info about client
 */
int Chunk_server::accept_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) {
    // 1. Accept pending TCP connection
    auto client = client_pool_ptr.find(client_fd);
    client->second.fd_pool.chunk_tcp_fd = accept(this->get_tcp_socket(),
                                                 (struct sockaddr *) &client->second.addr_pool.chunk_tcp_addr,
                                                 &client->second.addr_pool.chunk_tcp_addr_size);
    if (client->second.fd_pool.chunk_tcp_fd < 0) {
        std::cerr << "Error accepting client connection" << std::endl;
        return -1;
    }

    // 2. Storing info about client
//    mtx.lock();	    // locking other threads from accessing following data
//    // store new fd's and client address info for server
//    client_pool_ptr.insert(std::make_pair(client->second.fd_pool, client->second));
//    client_pool_ptr.erase(client);
//    client_fd.chunk_tcp_fd = client->second.fd_pool.chunk_tcp_fd;
//    mtx.unlock();	// unlocking data

    return client->second.fd_pool.chunk_tcp_fd;	// if everything is ok return client fd
}

/*
 * handle_tcp_client
 * 1. Init buffer for data transmission
 * 2. Read data from the client
 * 3. An error occurred or the client disconnected
 * 4. Remove the client from the list of connected clients
 */
void Chunk_server::handle_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) {
	// 1. Init buffer for data transmission
	char buffer[BUFFER_SIZE];
    memset(buffer,			// filling structure
		   0,				// with zeroes
		   BUFFER_SIZE);	// of size of said structure

    // 2. Read data from the client and send it back
    ssize_t bytes_read = 0;	// type used for size representation of buffer/array size
    while ((bytes_read = recv(client_pool_ptr[client_fd].fd_pool.chunk_tcp_fd,	// client file descriptor
							  buffer,				// our data buffer
							  BUFFER_SIZE,			// its size
							  0)) > 0) {			// while we are receiving more than zero bytes from client
        std::cout << "Received: " << buffer << std::endl;	// printing client data
        std::string response = buffer;
        send(client_pool_ptr[client_fd].fd_pool.chunk_tcp_fd,	// client fd
			 response.c_str(), 		// response
			 response.size(), 		// its size
			 0);					// send a response to the client
        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer with zeroes
    }

    // 3. An error occurred or the client disconnected
    if (bytes_read == 0) {
        std::cout << "Client disconnected" << std::endl;
    } else {
        std::cerr << "Error receiving data from client" << std::endl;	// standart error output stream
    }

    // 4. Remove the client from the list of connected clients
    // TODO: following code requires review
//    mtx.lock();  // by locking we make sure that only one thread
//    // is able to execute next command
//    clients.erase(client_pool_ptr[client_fd_pool].fd_pool.chunk_tcp_fd);	// remove function does not remove elements,
//    // only moving them to the end of the vector; returns iterator which points to first socket element. After that
//    // erase removes elements starting from remove iterator to the end of the vector
//    mtx.unlock();   // unlocking data
//    close(client_pool_ptr[client_fd_pool].fd_pool.chunk_tcp_fd); // close the client socket
}

/*
 * handle_udp_client
 * 1. Init buffer and client UDP structure
 * 2. Read data from the client and send it back
 * 3. Processing user data and sending it back (our game logic) TODO: needs to be fixed, data somehow jumps between threads
 * 4. An error occurred or the client disconnected
 */
void Chunk_server::handle_udp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) {
	// 1. Init buffer and client UDP structure
	char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);	// filling buffer with zeroes, we dont need memory trash in there	
//	struct sockaddr_in udp_client_addr;		// UDP client address and other stuff
//	socklen_t addr_len = sizeof(udp_client_addr);	// its size

	// 2. Read data from the client and send it back
	int bytes_read = 0;	
	while ((bytes_read = recvfrom(this->get_udp_socket(),	// fd we get from system to communicate with client
								 buffer, 					// our byte buffer, which contains client data
								 BUFFER_SIZE,				// its size
								 0, 						// some flags, uniportant right now
								 (struct sockaddr*) &client_pool_ptr[client_fd].addr_pool.chunk_udp_addr,	// here we reference to element of array, containing client connection data (IP, port etc.)
								 &client_pool_ptr[client_fd].addr_pool.chunk_udp_addr_size)) > 0) {		// while we are receiving more than zero bytes from client
        
		// 3. Processing user data and sending it back (our game logic) !!! needs to be fixed, data somehow jumps between threads
		for (char i : buffer) {
			if (i == 'w') client_pool_ptr[client_fd].character.update_position(1, 0, 0);
			if (i == 's') client_pool_ptr[client_fd].character.update_position(-1, 0, 0);
			if (i == 'a') client_pool_ptr[client_fd].character.update_position(0, -1, 0);
			if (i == 'd') client_pool_ptr[client_fd].character.update_position(0, 1, 0);
		}
        sendto(this->get_udp_socket(),	// send a response to the client; args are the same as in recvfrom, except addr_len is not pointer
               client_pool_ptr[client_fd].character.convert_to_buffer(),	// TODO: this should be fixed due to the inconsistency between P.convert_to_buffer() and BUFFER_SIZE
               client_pool_ptr[client_fd].character.get_buffer_length(),
			   0,
               (struct sockaddr*) &client_pool_ptr[client_fd].addr_pool.chunk_udp_addr,
               client_pool_ptr[client_fd].addr_pool.chunk_udp_addr_size);
        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer with zeroes
    }

    // 4. An error occurred or the client disconnected
    if (bytes_read == 0) {
        std::cout << "Client disconnected" << std::endl;
    } else {
        std::cerr << "Error receiving data from client" << std::endl;	// standart error output stream
    }
}

Chunk_server::~Chunk_server() {
	close(this->get_tcp_socket());
    close(this->get_udp_socket());
}
