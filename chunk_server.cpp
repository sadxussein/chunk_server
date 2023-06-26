#include "chunk_server.h"

chunk_server::chunk_server() {
}

/*
 * set_socket
 * 1. Create the server socket file descriptor (fd), it is main descriptor here
 * 2. Bind the server socket to a port
 * 3. Listen for incoming connections if TCP and save fd of our TCP connection
 * 4. Or just save our UDP connection fd
 */
int chunk_server::set_socket(int type, 
							 int port) {
	// 1. Create the server socket file descriptor, it is main descriptor here
    int server_socket = socket(AF_INET,	// AF_INET = internet tpc/ip
								type, 	// SOCK_STREAM = tcp, SOCK_DGRAM = udp
								0);		// 0 = protocol will be selected by the system automatically
    if (server_socket < 0) {
        cerr << "Error creating server socket" << endl;
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
        cerr << "Error binding server socket to port" << endl;
        return -1;
    }
	
	// 3. Listen for incoming connections if TCP and save fd of our TCP connection
    if (type == SOCK_STREAM) {
        server_tcp_socket_fd = server_socket;			// save our server fd for internal use
        if (listen(server_socket, MAX_CLIENTS) < 0) {	// major difference between TCP and UDP is that TCP has to LISTEN to port for connections
            cerr << "Error listening for incoming connections" << endl;
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
 * 3. Spawn thread for this client
 */
int chunk_server::accept_tcp_client() {
	// 1. Accepting pending TCP connections
	socklen_t addr_size;
	struct sockaddr_in tcp_client_addr;		// TCP client address and other stuff
	addr_size = sizeof(tcp_client_addr);	// its size
	int client_tcp_socket_fd = accept(this->get_tcp_socket(),	// we get client address info from accept; client_tcp_socket_fd stores file descriptor
									  (struct sockaddr *) &tcp_client_addr,
									  &addr_size);	
	if (client_tcp_socket_fd < 0) {
		cerr << "Error accepting client connection" << endl;
		return -1;
	}

	// 2. Storing info about client
	clients_mutex.lock();	// locking other threads from accessing following data
	clients_tcp_addresses.insert(make_pair(client_tcp_socket_fd, 
										   &tcp_client_addr));	// store fd and client address info for server
	clients_mutex.unlock();	// unlocking data
	
	// 3. Spawn thread for this client
	chunk_server::add_tcp_thread(client_tcp_socket_fd);
	
	return 0;
}

/*
 * add_tcp_thread
 */
void chunk_server::add_tcp_thread(int client_tcp_socket_fd) {
	thread t(&chunk_server::handle_tcp_client,	// pointer to function
				this, 							// object pointer
				client_tcp_socket_fd);			// function argument
	t.detach();									// now function will operate detached from others
}

/*
 * handle_tcp_client
 * 1. Init buffer for data transmission
 * 2. Read data from the client
 * 3. An error occurred or the client disconnected
 * 4. Remove the client from the list of connected clients
 */
void chunk_server::handle_tcp_client(int client_tcp_socket_fd) {
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
        cout << "Received: " << buffer << endl;	// printing client data
		chunk_server::chat.push_back(buffer);	// adding message to chat array
        string response = buffer;
        send(client_tcp_socket_fd,	// client fd
			 response.c_str(), 		// response
			 response.size(), 		// its size
			 0);					// send a response to the client
        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer with zeroes
    }

    // 3. An error occurred or the client disconnected
    if (bytes_read == 0) {
        cout << "Client disconnected" << endl;
    } else {
        cerr << "Error receiving data from client" << endl;	// standart error output stream
    }

    // 4. Remove the client from the list of connected clients
    clients_mutex.lock();								// by locking we make sure that only one thread is able to execute next command
    clients_tcp_addresses.erase(client_tcp_socket_fd);	// remove function does not remove elements, only moving them to the end of the vector; returns iterator which points to first socket element. After that erase removes elements starting from remove iterator to the end of the vector
    clients_mutex.unlock();								// unlocking data
    close(client_tcp_socket_fd);						// close the client socket
}

/*
 * add_udp_thread
 */
void chunk_server::add_udp_thread() {
	thread t(&chunk_server::handle_udp_client,	// pointer to function
			 this);							// object pointer
	t.detach();									// now function will operate detached from others
}

/*
 * handle_udp_client
 * 1. Init buffer and client UDP structure
 * 2. Read data from the client and send it back
 * 3. An error occurred or the client disconnected
 */
void chunk_server::handle_udp_client() {
	// 1. Init buffer and client UDP structure
	char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);	// filling buffer with zeroes, we dont need memory trash in there	
	struct sockaddr_in udp_client_addr;		// UDP client address and other stuff
	socklen_t addr_len = sizeof(udp_client_addr);	// its size	
	
	// 2. Read data from the client and send it back
	int bytes_read = 0;
	while (bytes_read = recvfrom(this->get_udp_socket(),	// fd we get from system to communicate with client
								 buffer, 					// our byte buffer, which contains client data
								 BUFFER_SIZE,				// its size
								 0, 						// some flags, uniportant right now
								 (struct sockaddr*) &udp_client_addr,	// here we reference to element of array, containing client connection data (IP, port etc.)
								 &addr_len) > 0) {		// while we are receiving more than zero bytes from client
        cout << "Received: " << buffer << endl;	// printing client data
        sendto(this->get_udp_socket(),	// send a response to the client; args are the same as in recvfrom, except addr_len is not pointer
			   buffer,
			   BUFFER_SIZE,
			   0,
			   (struct sockaddr*) &udp_client_addr,
			   addr_len); 
        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer with zeroes
    }

    // 3. An error occurred or the client disconnected
    if (bytes_read == 0) {
        cout << "Client disconnected" << endl;
    } else {
        cerr << "Error receiving data from client" << endl;	// standart error output stream
    }
}

int chunk_server::get_tcp_socket() {
    return server_tcp_socket_fd;
}

int chunk_server::get_udp_socket() {
    return server_udp_socket_fd;
}

chunk_server::~chunk_server() {	
	close(server_tcp_socket_fd);
    close(server_udp_socket_fd);
}