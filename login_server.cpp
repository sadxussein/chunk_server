#include "login_server.h"

/*
 * handle_tcp_client
 * 1. Init buffer for data transmission
 * 2. Read data from the client
 * 3. Validate username and password
 * 4. TODO: Update user status every 5 seconds. If he is not connected - remove user from pool, close connection with him
      and issue other servers that user went offline
 * TODO: need to work on adding user to player pool
 * 5. An error occurred or the client disconnected
 */
void Login_server::handle_tcp_client(int client_tcp_socket_fd) {
	// 1. Init buffer for data transmission
	char buffer[BUFFER_SIZE];
    memset(buffer,			// filling structure
		   0,				// with zeroes
		   BUFFER_SIZE);	// of size of said structure
		   
	std::string username;
    std::string password;

    // 2. Read data from the client, first line username, second line password
    ssize_t bytes_read = 0;	// type used for size representation of buffer/array size
	size_t lines_count = 0;	// line count, since we need only name/password we will not have more than two
	while (lines_count < 2 &&                       // while we dont get more than two lines
          (bytes_read = recv(client_tcp_socket_fd,	// client file descriptor
							  buffer,				// our data buffer
							  BUFFER_SIZE,			// its size
							  0)) > 0) {		    // while we are receiving more than zero bytes from client
		if (lines_count == 0) {         // reading user name
            std::cout << "Received user name: " << buffer << std::endl;
            username = buffer;
        } else if (lines_count == 1) {  // reading user password
            std::cout << "Received password hash: " << buffer << std::endl;
            password = buffer;
        }
		lines_count ++;
    }

	// 3. Validate username and password and send response to user
	std::string response;
	if (username == USERNAME && password == USERPASS) { // if user name and password is correct
        response = "Welcome back " + username + "!\n";	// since user is now validated and
    } else {                                            // if incorrect we remove user from pool and close connection
        response = "Please verify if user/password credentials are correct.\n";
        remove_user(client_tcp_socket_fd);
    }
    std::cout << response << std::endl;
	send(client_tcp_socket_fd,	// client fd
		 response.c_str(), 		// response
		 response.size(), 		// its size
		 0);					// send a response to the client
         memset(buffer, 0, BUFFER_SIZE); // resetting the buffer with zeroes

    // 4. TODO: Update user status every 5 seconds. If he is not connected - remove user from pool, close
    //    connection with him and issue other servers that user went offline
    while ((bytes_read = recv(client_tcp_socket_fd,	// client file descriptor
                              buffer,				// our data buffer
                              BUFFER_SIZE,			// its size
                              0)) > 0) {		    // while we are receiving more than zero bytes from client
        std::cout << "User is still online, heartbeat: " << buffer << std::endl;
    }

    // 5. An error occurred or the client disconnected remove him from clients list
    if (bytes_read == 0) {
        std::cout << "Client disconnected" << std::endl;
        remove_user(client_tcp_socket_fd);
    } else {
        std::cerr << "Error receiving data from client" << std::endl;	// standard error output stream
        remove_user(client_tcp_socket_fd);
    }
}

void Login_server::add_tcp_thread(int client_tcp_socket_fd) {
    // following lambda construct is required for calling non-static function
    std::thread t([this, client_tcp_socket_fd]() {	// object pointer and function argument
        handle_tcp_client(client_tcp_socket_fd);	// pointer to function
    });
    t.detach();	// now function will operate detached from others
}