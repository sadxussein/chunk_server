// Only commenting lines which are not selfexplanatory in their mnemonic.

#include <iostream>
#include <thread>
#include <mutex>	// mutual exclusion; provides access to protected and shared resources
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>	// posix syscalls
#include <sys/types.h>	// unix types for syscalls
#include <sys/socket.h>	// unix sockets
#include <netinet/in.h>	// unix types for network

using namespace std;

const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1500;

vector<int> clients;
mutex clients_mutex;

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);	// filling buffer with zeroes, we dont need memory trash in there

    // --- Read data from the client ---
    ssize_t bytes_read = 0;	// type used for size representation of buffer/array size
    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {	// while we are receiving more than zero bytes from client
        buffer[bytes_read] = '\0';	// adding 'end of line' to recieved bytes array
        cout << "Received: " << buffer << endl;	// printing client data        
        string response = "Hello, client!";
        send(client_socket, response.c_str(), response.size(), 0); // send a response to the client      
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
    clients.erase(remove(clients.begin(), clients.end(), client_socket), clients.end());	// remove function does not remove elements, only moving them to the end of the vector; returns iterator which points to first client_socket element. After that erase removes elements starting from remove iterator to the end of the vector
    clients_mutex.unlock();
    
    close(client_socket);	// close the client socket
}

int main(int argc, char* argv[]) {
    // --- Create the server socket ---
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);	// AF_INET = tpc/ip, SOCK_STREAM = tcp, 0 = protocol will be selected by the system automatically; int here is the file descriptor
    if (server_socket < 0) {
        cerr << "Error creating server socket" << endl;
        return -1;
    }

    // --- Bind the server socket to a port ---
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));	// zeroing structure
    address.sin_family = AF_INET;	// still tcp/ip
    address.sin_addr.s_addr = INADDR_ANY;	// system will decide which address to use
    address.sin_port = htons(55055);	// htons is important transformation of byte order from Little endian to Big endian

    if (bind(server_socket, (struct sockaddr*) &address, sizeof(address)) < 0) {	// tying socket file descriptor and server local address
        cerr << "Error binding server socket to port" << endl;
        return -1;
    }

    // --- Listen for incoming connections ---
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        cerr << "Error listening for incoming connections" << endl;
        return -1;
    }

    // --- Accept new clients in an infinite loop ---
    while (true) {
        // --- Wait for a client to connect ---
        int client_socket = accept(server_socket, NULL, NULL);	// we don't care about client address information, hence the NULL
        if (client_socket < 0) {
            cerr << "Error accepting client connection" << std::endl;
            continue; // try again
        }

        // --- Add the new client to the list of connected clients ---
        clients_mutex.lock();
        clients.push_back(client_socket);
        clients_mutex.unlock();

        // --- Spawn a new thread to handle this client ---
        thread t(handle_client, client_socket);
        t.detach();
    }
    
    close(server_socket); // сlose the server socket

    return 0;	// ASCII standart requirment
}