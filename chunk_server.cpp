#include "chunk_server.h"

Chunk_server::Chunk_server() {}

/*
 * handle_tcp_client
 * 1. Init buffer for data transmission
 * 2. Read data from the client
 * 3. An error occurred or the client disconnected
 * 4. Remove the client from the list of connected clients
 */
void Chunk_server::handle_tcp_client(int client_tcp_socket_fd) {
	// 1. Init buffer for data transmission
	char buffer[BUFFER_SIZE];
    memset(buffer,			// filling structure
		   0,				// with zeroes
		   BUFFER_SIZE);	// of size of said structure

    // 2. Read data from the client and send it back
    ssize_t bytes_read = 0;	// type used for size representation of buffer/array size
    while ((bytes_read = recv(client_tcp_socket_fd,	// client file descriptor
							  buffer,				// our data buffer
							  BUFFER_SIZE,			// its size
							  0)) > 0) {			// while we are receiving more than zero bytes from client
        std::cout << "Received: " << buffer << std::endl;	// printing client data
        std::string response = buffer;
        send(client_tcp_socket_fd,	// client fd
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
    mtx.lock();								// by locking we make sure that only one thread is able to execute next command
    clients.erase(client_tcp_socket_fd);	// remove function does not remove elements, only moving them to the end of the vector; returns iterator which points to first socket element. After that erase removes elements starting from remove iterator to the end of the vector
    mtx.unlock();							// unlocking data
    close(client_tcp_socket_fd);			// close the client socket
}

/*
 * handle_udp_client
 * 1. Init buffer and client UDP structure
 * 2. Read data from the client and send it back
 * 3. Processing user data and sending it back (our game logic) TODO: needs to be fixed, data somehow jumps between threads
 * 4. An error occurred or the client disconnected
 */
void Chunk_server::handle_udp_client() {
	// 1. Init buffer and client UDP structure
	char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);	// filling buffer with zeroes, we dont need memory trash in there	
	struct sockaddr_in udp_client_addr;		// UDP client address and other stuff
	socklen_t addr_len = sizeof(udp_client_addr);	// its size	
	Character P; // our player instance
	
	// 2. Read data from the client and send it back
	int bytes_read = 0;	
	while ((bytes_read = recvfrom(this->get_udp_socket(),	// fd we get from system to communicate with client
								 buffer, 					// our byte buffer, which contains client data
								 BUFFER_SIZE,				// its size
								 0, 						// some flags, uniportant right now
								 (struct sockaddr*) &udp_client_addr,	// here we reference to element of array, containing client connection data (IP, port etc.)
								 &addr_len)) > 0) {		// while we are receiving more than zero bytes from client
        
		// 3. Processing user data and sending it back (our game logic) !!! needs to be fixed, data somehow jumps between threads
		for (int i=0; i<1500; i++) {
			if (buffer[i] == 'w') P.update_position(1, 0, 0);
			if (buffer[i] == 's') P.update_position(-1, 0, 0);
			if (buffer[i] == 'a') P.update_position(0, -1, 0);
			if (buffer[i] == 'd') P.update_position(0, 1, 0);
		}
		P.show_position();	// printing players location in server terminal
        sendto(this->get_udp_socket(),	// send a response to the client; args are the same as in recvfrom, except addr_len is not pointer
			   P.convert_to_buffer(),	// TODO: this should be fixed due to the inconsistency between P.convert_to_buffer() and BUFFER_SIZE
			   P.get_buffer_length(),
			   0,
			   (struct sockaddr*) &udp_client_addr,
			   addr_len); 
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
