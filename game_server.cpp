#include "game_server.h"

/*
 * accept_tcp_client
 * 1. Accept pending TCP connection
 * 2. Storing info about client
 */
int Game_server::accept_tcp_client(std::map<Client_fd_pool, Client> &client_pool_ptr, Client_fd_pool &client_fd_pool) {
    // 1. Accept pending TCP connection
    std::cout << "-1" << std::endl;
    auto client = client_pool_ptr.find(client_fd_pool);
    std::cout << "client iterator " << client->second.fdPool.login_tcp_fd << std::endl;
    std::cout << "0" << std::endl;
    client->second.fdPool.game_tcp_fd = accept(this->get_tcp_socket(),
                                               (struct sockaddr *) &client->second.addr,        // perhaps?
                                               &client->second.addr_size);
    std::cout << "1" << std::endl;
    if (client->second.fdPool.game_tcp_fd < 0) {
        std::cerr << "Error accepting client connection" << std::endl;
        return -1;
    }

    std::cout << "2" << std::endl;
    // 2. Storing info about client
    mtx.lock();	    // locking other threads from accessing following data
    // store new fd's and client address info for server
    client_pool_ptr.insert(std::make_pair(client->second.fdPool, client->second));
    client_fd_pool.game_tcp_fd = client->second.fdPool.game_tcp_fd;
    mtx.unlock();	// unlocking data

    std::cout << "3" << std::endl;
    return client_fd_pool.game_tcp_fd;	// if everything is ok return client fd
}

// TODO
void Game_server::handle_tcp_client(int client_tcp_socket_fd) {
    // 1. Reading data from client
    char buffer[BUFFER_SIZE];
    // reading single line from user containing character index
    ssize_t bytes_read = read(client_tcp_socket_fd, buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
        std::cerr << "Error reading character index from client" << std::endl;
    }

    // 2. Connecting to database to fetch client's characters
    // commentary on pqxx part in Login_server::handle_tcp_client
//    Game_server::postgres_connect();
//    pqxx::work transaction(*Game_server::connect);
//    pqxx::result result = transaction.exec("SELECT * FROM characters");      // TODO: correct query is mandatory
//    std::stringstream ss;   // TODO: temp, just for testing
//    if (!result.empty()) {
//        for (const auto& row: result) {
//            std::cout << row["id"].as<std::string>() << ' '
//                      << row["name"].as<std::string>() << ' '
//                      << row["owner_id"].as<std::string>() << std::endl;
//            ss << row["id"].as<std::string>() << ' '
//               << row["name"].as<std::string>() << ' '
//               << row["owner_id"].as<std::string>() << std::endl;
//        }
//    }
//    Game_server::postgres_disconnect();
//    std::cout << "Game server disconnected from database" << std::endl;

    // 3. Sending response to client TODO: temporary test solution
    std::string response = "you have selected some character; WORK IN PROGRESS\n";
    send(client_tcp_socket_fd, response.c_str(), response.size(), 0);
}

// TODO
void Game_server::handle_udp_client() {
    std::cout << "This class doesn't implement UDP" << std::endl;
}

Game_server::~Game_server() {
    close(this->get_tcp_socket());
}