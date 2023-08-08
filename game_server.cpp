#include <unordered_map>
#include "game_server.h"

/*
 * accept_tcp_client
 * 1. Accept pending TCP connection
 * 2. Storing info about client
 */
int Game_server::accept_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) {
    // 1. Accept pending TCP connection
    auto client = client_pool_ptr.find(client_fd);
    client->second.fd_pool.game_tcp_fd = accept(this->get_tcp_socket(),
                                                (struct sockaddr *) &client->second.addr_pool.game_tcp_addr,        // perhaps?
                                                &client->second.addr_pool.game_tcp_addr_size);
    if (client->second.fd_pool.game_tcp_fd < 0) {
        std::cerr << "Error accepting client connection" << std::endl;
        return -1;
    }

//    // 2. Storing info about client
//    mtx.lock();	    // locking other threads from accessing following data
//    // store new fd's and client address info for server

//    mtx.unlock();	// unlocking data

    return client->second.fd_pool.game_tcp_fd;	// if everything is ok return client fd
}

/*
 * handle_tcp_client
 * 1. Connecting to database to fetch client's characters
 * 2. Sending to client a list of characters
 * 3. Reading character index from client
 * 4. Validating user input by checking database for existent characters and serializing it if it exists
 * 5. Setting character position, getting data from DB
 * 6. Disconnect from database
 */
void Game_server::handle_tcp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) {
    // 1. Connecting to database to fetch client's characters
    // commentary on pqxx part in Login_server::handle_tcp_client
    // TODO: connection verification and exception handling
    Game_server::postgres_connect();
    pqxx::work full_character_pool_transaction(*Game_server::connect);
    pqxx::result full_character_pool = full_character_pool_transaction.exec("SELECT * FROM characters;");      // TODO: correct query is mandatory
    std::stringstream ss;   // preparing data for sending to user
    if (!full_character_pool.empty()) {
        for (const auto& character: full_character_pool) {
            if (character["owner_id"].as<size_t>() == client_pool_ptr[client_fd].id) {
                // TODO: remove, testing
                std::cout << character["id"].as<std::string>() << ' '
                          << character["name"].as<std::string>() << std::endl;
                ss << character["id"].as<std::string>() << ' '
                   << character["name"].as<std::string>() << std::endl;
            }
        }
    }
    full_character_pool_transaction.commit();

    // 2. Sending to client a list of characters
    send(client_pool_ptr[client_fd].fd_pool.game_tcp_fd, ss.str().c_str(), ss.str().size(), 0);

    // 3. Reading character index from client
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    // reading single line from user containing character index
    ssize_t bytes_read = read(client_pool_ptr[client_fd].fd_pool.game_tcp_fd, buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
        std::cerr << "Error reading character index from client" << std::endl;
    }

    // 4. Validating user input by checking database for existent characters and serializing it if it exists
    // TODO: consider using previous query, next one might be redundant, since we already have characters list in
    //  previous result variable
    ss.str("");
    std::cout << "Buffer, client query to select character: ";  // TODO: remove test line
    for (char c: buffer) {
        std::cout << c;     // TODO: remove test line
        // insert buffer into stringstream variable for upcoming validation
        ss << c;
    }
    std::cout << std::endl;     // TODO: remove test line
    std::string query = "SELECT * FROM characters WHERE owner_id='"
            + std::to_string(client_pool_ptr[client_fd].id)
            + "' AND id=" + ss.str() + ";";
    std::cout << "SQL query: " << query << std::endl;   // TODO: remove test line
    // try to query the database
    // TODO: assign selected character to client and send him to chunk
    try {
        pqxx::work client_character_transaction(*Game_server::connect);     // TODO: same as below
        pqxx::result client_character;  // TODO: duplicating previous pqxx::result variable, consider removal
        client_character = client_character_transaction.exec(query);
        // TODO: validate client character index input
        if (!client_character.empty()) {
            // TODO: since row is always one (if table is OK) consider replacing loop with front() or at(0)
            for (const auto& row: client_character) {
                // prepare stringstream variable for sending to client
                ss.str("");
                ss << row["id"].as<std::string>() << ' ' << row["name"].as<std::string>() << ' '
                   << row["class_id"].as<std::string>() << ' ' << row["race_id"].as<std::string>() << std::endl
                   << row["experience_points"].as<std::string>() << ' ' << row["level"].as<std::string>() << std::endl
                   << row["current_health"].as<std::string>() << ' ' << row["current_mana"].as<std::string>();
                // and send client his character pool
                if (send(client_pool_ptr[client_fd].fd_pool.game_tcp_fd,
                         ss.str().c_str(), ss.str().size(), 0) < 0) {
                    std::cerr << "Error sending to client character stats" << std::endl;
                }
                // set client's selected character, serializing
                client_pool_ptr[client_fd].character.setId(row["id"].as<std::size_t>());
                client_pool_ptr[client_fd].character.setName(row["name"].as<std::string>());
                client_pool_ptr[client_fd].character.setClassId(row["class_id"].as<std::size_t>());
                client_pool_ptr[client_fd].character.setRaceId(row["race_id"].as<std::size_t>());
                client_pool_ptr[client_fd].character.setExperiencePoints(row["experience_points"].as<std::size_t>());
                client_pool_ptr[client_fd].character.setLevel(row["level"].as<std::size_t>());
                client_pool_ptr[client_fd].character.setCurrentHealth(row["current_health"].as<std::size_t>());
                client_pool_ptr[client_fd].character.setCurrentMana(row["current_mana"].as<std::size_t>());
            }
        }
        // commit only AFTER work is done
        client_character_transaction.commit();
    // catch exception if we fail
    } catch (const pqxx::sql_error& e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
    }

    // 5. Setting character position, getting data from DB
    ss.str("");
    query = "SELECT * FROM character_position WHERE character_id="
            + std::to_string(client_pool_ptr[client_fd].character.getId()) + ";";
    try {
        pqxx::work cli_char_pos_transaction(*Game_server::connect);
        pqxx::result cli_char_pos;
        cli_char_pos = cli_char_pos_transaction.exec(query);
        if (!cli_char_pos.empty()) {
            ss << cli_char_pos.front().at("x").as<float>() << ' '
               << cli_char_pos.front().at("y").as<float>() << ' '
               << cli_char_pos.front().at("y").as<float>() << std::endl;
            std::cout << ss.str();      // TODO: remove debug line
            if (send(client_pool_ptr[client_fd].fd_pool.game_tcp_fd,
                     ss.str().c_str(), ss.str().size(), 0) < 0) {
                std::cerr << "Error sending to client character stats" << std::endl;
            }
            client_pool_ptr[client_fd].character.setPosition(cli_char_pos.front().at("x").as<float>(),
                                                             cli_char_pos.front().at("y").as<float>(),
                                                             cli_char_pos.front().at("y").as<float>());
        }
        cli_char_pos_transaction.commit();
    } catch (const pqxx::sql_error& e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
    }

    // 6. Disconnect from database
    Game_server::postgres_disconnect();
}

// TODO
void Game_server::handle_udp_client(std::unordered_map<int, Client> &client_pool_ptr, int client_fd) {
    std::cout << "This class doesn't implement UDP" << std::endl;
}

Game_server::~Game_server() {
    close(this->get_tcp_socket());
}