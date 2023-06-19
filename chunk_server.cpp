#include "chunk_server.h"

chunk_server::chunk_server() {
}

int chunk_server::init_socket(int type, int port) {	
	// --- Create the server socket ---
    int server_socket = socket(AF_INET, type, 0);	// AF_INET = internet tpc_ip, type = tcp/udp (SOCK_STREAM, SOCK_DGRAM, 0 = protocol will be selected by the system automatically; int here is the file descriptor
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
	if (listen(server_socket, MAX_CLIENTS) < 0) {
        cerr << "Error listening for incoming connections" << endl;
        return -1;
    }
	
	sockets.push_back(server_socket);
	
	return server_socket;
}

vector<int>* chunk_server::get_sockets() {
	return &sockets;
}	

chunk_server::~chunk_server() {
	for (int socket : sockets) {
		close(socket);
	}
}