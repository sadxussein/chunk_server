#include "login_server.h"

#include <openssl/sha.h>

/*
 * login()
 */
Login_server::login() {
	std::cout << "Enter user name: ";
	string uname, passwd;
	std::cin >> uname;
	std::cout << "Enter user password: ";
	std::cin >> passwd;
	if (uname == Login_server::USERNAME && hash_password(passwd) == passwd) {
		std::cout << "Welcome back, %USERNAME%" << std::endl;
	}
}

/*
 * hash_password
 */
std::string Login_server::hash_password(const std::string& password) {
	unsigned char hash[SHA256_DIGEST_LENGTH];	// sha header provided constant
    SHA256_CTX sha256Context;					// sha context variable
	
    SHA256_Init(&sha256Context);				// init with pointer
    SHA256_Update(&sha256Context, input.c_str(), input.length());	// input our password string
    SHA256_Final(hash, &sha256Context);			// finalize
	
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

/*
 * handle_tcp_client
 * 1. Init buffer for data transmission
 * 2. Read data from the client
 * 3. Validate username and password
 * !!! need to work on adding user to player pool
 * 4. An error occurred or the client disconnected
 * 5. Remove the client from the list of connected clients
 */
void Login_server::handle_tcp_client(int client_tcp_socket_fd) {
	// 1. Init buffer for data transmission
	char buffer[BUFFER_SIZE];
    memset(buffer,			// filling structure
		   0,				// with zeroes
		   BUFFER_SIZE);	// of size of said structure
		   
	string username, password;

    // 2. Read data from the client, first line username, second line password
    ssize_t bytes_read = 0;	// type used for size representation of buffer/array size
	size_t lines_count = 0;	// line count, since we need only name/password we will not have more than two
	while ((bytes_read = recv(client_tcp_socket_fd,	// client file descriptor
							  buffer,				// our data buffer
							  BUFFER_SIZE,			// its size
							  0)) > 0 &&			// while we are receiving more than zero bytes from client
							  lines_count < 2) {	// and while we dont get more than two lines
		if (lines_count == 0)
			cout << "Recieved user name: " << buffer << endl;
			username = buffer;
		else if (lines_count == 1)
			cout << "Recieved password hash: " << buffer << endl;
			password = buffer;        
		lines_count ++;
    }
	
	// 3. Validate username and password and send response to user
	string response;
	if (username == USERNAME && password == USERPASS)
		response = "Welcome back " + username + "!\n";	// !!! need to work on adding user to player pool
	else
		response = "Please verify if user/password credentials are correct.\n
	send(client_tcp_socket_fd,	// client fd
		 response.c_str(), 		// response
		 response.size(), 		// its size
		 0);					// send a response to the client
         memset(buffer, 0, BUFFER_SIZE); // resetting the buffer with zeroes

    
    // 4. An error occurred or the client disconnected
    if (bytes_read == 0) {
        cout << "Client disconnected" << endl;
    } else {
        cerr << "Error receiving data from client" << endl;	// standart error output stream
    }

    // 5. Remove the client from the list of connected clients
    clients_mutex.lock();								// by locking we make sure that only one thread is able to execute next command
    clients_tcp_addresses.erase(client_tcp_socket_fd);	// remove function does not remove elements, only moving them to the end of the vector; returns iterator which points to first socket element. After that erase removes elements starting from remove iterator to the end of the vector
    clients_mutex.unlock();								// unlocking data
    close(client_tcp_socket_fd);						// close the client socket
}