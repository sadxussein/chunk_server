#include "login_server.h"

Login_server::Login_server() {
    userExists = false;
}

/*
 * handle_tcp_client override
 * 1. Init buffer for data transmission
 * 2. Read data from the client
 * 3. Connecting to postgres
 * 4. Check if user exists in table and password is correct
 * 5. Close connection to postgres
 * 6. Validate username and userpass and send response to user
 * 7. TODO: Update user status every 5 seconds. If he is not connected -
 *      remove user from pool, close connection with him
 *      and issue other servers that user went offline
 * TODO: need to work on adding user to player pool
 * 8. An error occurred or the client disconnected
 */
void Login_server::handle_tcp_client(int client_tcp_socket_fd) {
	// 1. Init buffer for data transmission
	char buffer[BUFFER_SIZE];
    memset(buffer,			// filling structure
		   0,				// with zeroes
		   BUFFER_SIZE);	// of size of said structure
		   
    // 2. Read data from the client, first line username, second line userpass
    std::string username;
    std::string userpass;
    ssize_t bytes_read = 0;	// type used for size representation of buffer/array size
	size_t lines_count = 0;	// line count, since we need only name/userpass we will not have more than two
	while (lines_count < 2 &&                       // while we dont get more than two lines
           (bytes_read = recv(client_tcp_socket_fd, // client file descriptor
							  buffer,               // our data buffer
							  BUFFER_SIZE,          // its size
							  0)) > 0) {            // while we are receiving more than zero bytes from client
		if (lines_count == 0) {         // reading user name
            std::cout << "Received user name: " << buffer << std::endl;
            username = buffer;
        } else if (lines_count == 1) {  // reading user userpass
            std::cout << "Received userpass hash: " << buffer << std::endl;
            userpass = buffer;
        }
		lines_count ++;
    }

    // 3. Connecting to postgres
    Login_server::postgres_connect();

    // 4. Check if user exists in table and password is correct
    // create SQL-queries handle object
    pqxx::work transaction(*Login_server::connect);
    // get users table
    // TODO: make correct SQL request, search for user directly
    pqxx::result result = transaction.exec("SELECT * FROM users");
    // check if user exists in table
    if (!result.empty()) {
        userExists = false;
        for (const auto& row: result) {
            std::cout << row["login"].as<std::string>() << ' ' << row["password"].as<std::string>() << std::endl;
            // the actual comparison between table and user input
            if (row["login"].as<std::string>() == username && row["password"].as<std::string>() == userpass) {
                userExists = true;
                break;
            }
        }
    }

    // 5. Close connection to postgres
    Login_server::postgres_disconnect();

	// 6. Validate username and userpass and send response to user
	std::string response;
    // TODO: in case if user doesn't enter name/pass server breaks, fix it
	if (userExists) {                                   // if user name and userpass is correct
        response = "Welcome back " + username + "!\n";	// since user is now validated and
        std::cout << response << std::endl;
        send(client_tcp_socket_fd,	// client fd
             response.c_str(), 		// response
             response.size(), 		// its size
             0);					// send a response to the client
        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer with zeroes

        // 7. TODO: Update user status every 5 seconds. If he is not connected - remove user from pool, close
        //     connection with him and issue other servers that user went offline
        //     server has to ping client, not vice-versa - THINK
        while ((bytes_read = recv(client_tcp_socket_fd,	// client file descriptor
                                  buffer,				// our data buffer
                                  BUFFER_SIZE,			// its size
                                  0)) > 0) {		    // while we are receiving more than zero bytes from client
            std::cout << "User is still online, heartbeat: " << buffer << std::endl;
        }

        // 8. An error occurred or the client disconnected remove him from clients list
        if (bytes_read == 0) {
            std::cout << "Client disconnected" << std::endl;
            remove_client(client_tcp_socket_fd);
        } else {
            std::cerr << "Error receiving data from client" << std::endl;	// standard error output stream
            remove_client(client_tcp_socket_fd);
        }
    // TODO: fix incorrect input by user
    } else {    // if incorrect we remove user from pool and close connection
        response = "Please verify if user/userpass credentials are correct.\n";
        std::cout << response << std::endl;
        send(client_tcp_socket_fd,	// client fd
             response.c_str(), 		// response
             response.size(), 		// its size
             0);					// send a response to the client
    }
}

/*
 * handle_udp_client override
 */
void Login_server::handle_udp_client() {
    std::cout << "This server class does not support UDP" << std::endl;
}

Login_server::~Login_server() {
    close(this->get_tcp_socket());
}