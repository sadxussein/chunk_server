#include "chunk_server.h"

chunk_server::chunk_server() {
}

int chunk_server::set_socket(int type, int port) {	
	// --- Create the server socket ---
    int server_socket = socket(AF_INET, type, 0);	// AF_INET = internet tpc_ip, SOCK_STREAM || SOCK_DGRAM = tcp || udp, 0 = protocol will be selected by the system automatically; int here is the file descriptor
    if (server_socket < 0) {
        cerr << "Error creating server socket" << endl;
        return -1;
    }

    // --- Bind the server socket to a port ---
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));	// zeroing structure
    address.sin_family = AF_INET;	// still tcp/ip
    address.sin_addr.s_addr = INADDR_ANY;	// system will decide which address to use
    address.sin_port = htons(port);	// htons is important transformation of byte order from Little endian to Big endian
    if (bind(server_socket, (struct sockaddr*) &address, sizeof(address)) < 0) {	// tying socket file descriptor and server local address
        cerr << "Error binding server socket to port" << endl;
        return -1;
    }
	
	// --- Listen for incoming connections ---
    if (type == SOCK_STREAM) {  // set attribute according to function input
        tcp_socket = server_socket;
        if (listen(server_socket, MAX_CLIENTS) < 0) {
            cerr << "Error listening for incoming connections" << endl;
            return -1;
        }
    }
    else if (type == SOCK_DGRAM) {
        udp_socket = server_socket;
    }
	
	return 0;
}

int chunk_server::get_tcp_socket() {
    return tcp_socket;
}

int chunk_server::get_udp_socket() {
    return udp_socket;
}

void chunk_server::handle_tcp_client(int socket) {
	char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);	// filling buffer with zeroes, we dont need memory trash in there

    // --- Read data from the client ---
    ssize_t bytes_read = 0;	// type used for size representation of buffer/array size
    while ((bytes_read = recv(socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {	// while we are receiving more than zero bytes from client
        buffer[bytes_read] = '\0';	// adding 'end of line' to recieved bytes array
        cout << "Received: " << buffer << endl;	// printing client data
		chunk_server::chat.push_back(buffer);	// adding message to chat array
        string response = "Hello, client!";
        send(socket, response.c_str(), response.size(), 0); // send a response to the client
        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }

    // --- An error occurred or the client disconnected ---
    if (bytes_read == 0) {
        cout << "Client disconnected" << endl;
    } else {
        cerr << "Error receiving data from client" << endl;	// standart error output stream
    }

    // --- Remove the client from the list of connected clients ---
    clients_mutex.lock();	// by locking we make sure that only one thread is able to execute next command
    clients.erase(remove(clients.begin(), clients.end(), socket), clients.end());	// remove function does not remove elements, only moving them to the end of the vector; returns iterator which points to first socket element. After that erase removes elements starting from remove iterator to the end of the vector
    clients_mutex.unlock();
    close(socket);	// close the client socket
}

void chunk_server::handle_udp_client(int socket) {
	/*char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);	// filling buffer with zeroes, we dont need memory trash in there
	
	ssize_t bytes_read = 0;
	while ((bytes_read = recvfrom(socket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*) &clients_addresses[socket], sizeof(clients_addresses[socket]))) > 0) {	// while we are receiving more than zero bytes from client
        buffer[bytes_read] = '\0';	// adding 'end of line' to recieved bytes array
        cout << "Received: " << buffer << endl;	// printing client data
		chat.push_back(buffer);	// adding message to chat array
        string response = "Hello, client!";
        sendto(socket, response.c_str(), response.size(), 0, (struct sockaddr*) &clients_addresses[socket], sizeof(clients_addresses[socket])); // send a response to the client
        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }

    // --- An error occurred or the client disconnected ---
    if (bytes_read == 0) {
        cout << "Client disconnected" << endl;
    } else {
        cerr << "Error receiving data from client" << endl;	// standart error output stream
    }*/
}

void chunk_server::add_tcp_client(int client_tcp_socket, struct sockaddr* client_addr) {
	chunk_server::clients_mutex.lock();
	chunk_server::clients.push_back(client_tcp_socket);	// store file descriptor which now operates with client
	chunk_server::clients_addresses.insert(make_pair(client_tcp_socket, client_addr));	// store client address info for server
	chunk_server::clients_mutex.unlock();
}

void chunk_server::add_udp_client(int client_tcp_socket, struct sockaddr* client_addr) {
	
}

void chunk_server::add_thread(int client_tcp_socket) {
	thread t(&chunk_server::handle_tcp_client, this, client_tcp_socket);
	t.detach();
}

chunk_server::~chunk_server() {	
	close(tcp_socket);
    close(udp_socket);
}