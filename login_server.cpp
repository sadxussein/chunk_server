#include <unordered_map>
#include "login_server.h"

Login_server::Login_server() {
    userExists = false;
}

/*
 * accept_tcp_client
 * 1. Accept pending initial TCP connection
 * 2. Storing info about client
 */
int Login_server::accept_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) {
    // 1. Accept pending initial TCP connection
    Client client;      // since user is not yet present in the system create one
    client.fd_pool.login_tcp_fd = accept(this->get_tcp_socket(),
                                         (struct sockaddr *) &client.addr_pool.login_tcp_addr,
                                         &client.addr_pool.login_tcp_addr_size);
    if (client.fd_pool.login_tcp_fd < 0) {
        std::cerr << "Error accepting client connection" << std::endl;
        return -1;
    }

    // 2. Storing info about client
    mtx.lock();	    // locking other threads from accessing following data
    // client-defining variable will be his login server file descriptor
    client_pool_ptr.insert(std::make_pair(client.fd_pool.login_tcp_fd, client));
    mtx.unlock();	// unlocking data

    return client.fd_pool.login_tcp_fd;	// if everything is ok return client fd
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
 * // TODO: store thread ID in client_pool
 */
void Login_server::handle_tcp_client(std::unordered_map<int, Client> &client_pool_ptr,
                                     int client_fd) {
	// 1. Init buffer for data transmission
	char buffer[BUFFER_SIZE];
    memset(buffer,			// filling structure
		   0,				// with zeroes
		   BUFFER_SIZE);	// of size of said structure
		   
    // 2. Read data from the client, first line username, second line userpass
    std::string username;
    std::string userpass;
    ssize_t bytes_read = 0;	// type used for size representation of buffer/array size
    while (username.empty()) {
        if (recv(client_fd, buffer, BUFFER_SIZE, 0) > 0) {
            username = buffer;
        }
    }
    while (userpass.empty()) {
        if (recv(client_fd, buffer,  BUFFER_SIZE, 0) > 0) {
            userpass = buffer;
        }
    }

//	size_t lines_count = 0;	// line count, since we need only name/userpass we will not have more than two
    // 2 lines from client - username and password
//	while (recv(client_pool_ptr[client_fd_pool].fd_pool.login_tcp_fd, buffer,  BUFFER_SIZE, 0) > 0) {
//        std::cout << bytes_read << " " << lines_count << " "
//                  << client_pool_ptr[client_fd_pool].fd_pool.login_tcp_fd << std::endl;
//		if (lines_count == 0) {         // reading user name
//            std::cout << "Received user name: " << buffer << std::endl;
//            username = buffer;
//        } else if (lines_count == 1) {  // reading user userpass
//            std::cout << "Received userpass hash: " << buffer << std::endl;
//            userpass = buffer;
//        } else {
//            break;
//        }
//		lines_count ++;
//    }

    // 3. Connecting to postgres
    // TODO: connection verification and exception handling
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
                client_pool_ptr[client_fd].id = row["id"].as<size_t>();
                userExists = true;
                break;
            }
        }
    }
    // sending request to the database
    transaction.commit();
    // 5. Close connection to postgres
    Login_server::postgres_disconnect();

	// 6. Validate username and userpass and send response to user
	std::string response;
    // TODO: in case if user doesn't enter name/pass server breaks, fix it
	if (userExists) {                                   // if username and userpass is correct
        response = "Welcome back " + username + "!\n";	// since user is now validated and
        std::cout << response << std::endl;
        send(client_pool_ptr[client_fd].fd_pool.login_tcp_fd,	// client fd
             response.c_str(), 		// response
             response.size(), 		// its size
             0);					// send a response to the client
        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer with zeroes

        // 7. TODO: Update user status every 5 seconds. If he is not connected - remove user from pool, close
        //     connection with him and issue other servers that user went offline
        //     server has to ping client, not vice-versa - THINK
        while ((bytes_read = recv(client_pool_ptr[client_fd].fd_pool.login_tcp_fd,	// client file descriptor
                                  buffer,				// our data buffer
                                  BUFFER_SIZE,			// its size
                                  0)) > 0) {		    // while we are receiving more than zero bytes from client
            std::cout << "User is still online, heartbeat: " << buffer << std::endl;
            std::cout << "User login tcp fd: " << client_pool_ptr[client_fd].fd_pool.login_tcp_fd << std::endl;
            for (const auto& c: client_pool_ptr) {
                std::cout << c.first << " client login tcp fd: " << c.second.fd_pool.login_tcp_fd << ' '
                          << "client chunk fd: " << c.second.fd_pool.chunk_tcp_fd << ' '
                          << "client game fd: " << c.second.fd_pool.game_tcp_fd << std::endl;
                std::cout << "IP/port: " << inet_ntoa(c.second.addr_pool.login_tcp_addr.sin_addr)
                          << ' ' << c.second.addr_pool.login_tcp_addr.sin_port << std::endl;
            }
        }

        // 8. An error occurred or the client disconnected remove him from clients list
        if (bytes_read == 0) {
            std::cout << "Client disconnected" << std::endl;
//            remove_client(client_pool_ptr[client_fd_pool].fd_pool.login_tcp_fd);   TODO: consider removal, local map
        } else {
            	// standard error output stream
            std::cerr << "Error receiving heartbeat from client, bytes read: " << bytes_read << std::endl;
            std::cout << "User login tcp fd: " << client_pool_ptr[client_fd].fd_pool.login_tcp_fd << std::endl;
            for (const auto& c: client_pool_ptr) {
                std::cout << c.first << " client login tcp fd: " << c.second.fd_pool.login_tcp_fd << ' '
                          << "client chunk fd: " << c.second.fd_pool.chunk_tcp_fd << ' '
                          << "client game fd: " << c.second.fd_pool.game_tcp_fd << std::endl;
                std::cout << "IP/port: " << inet_ntoa(c.second.addr_pool.login_tcp_addr.sin_addr)
                          << ' ' << c.second.addr_pool.login_tcp_addr.sin_port << std::endl;
            }
        }
    // TODO: fix incorrect input by user
    } else {    // if incorrect we remove user from pool and close connection
        response = "Please verify if user/userpass credentials are correct.\n";
        std::cout << response << std::endl;
        send(client_pool_ptr[client_fd].fd_pool.login_tcp_fd,	// client fd
             response.c_str(), 		// response
             response.size(), 		// its size
             0);					// send a response to the client
    }
}

/*
 * handle_udp_client override
 */
void
Login_server::handle_udp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) {
    std::cout << "This server class does not support UDP" << std::endl;
}

Login_server::~Login_server() {
    close(this->get_tcp_socket());
}


