#include "server.h"

// Temporary user name and sha256 hashed qwerty123 password
const char USERNAME = "player"
const char USERPASS = "d70e25e734dedbd1ca52fe81459b23af88f82819d06a823e07a79a86955e8ac8"

class Login_server : public Server {
public:
	void login();
	std::string hash_password(const std::string& password);
	void handle_tcp_client(int client_tcp_socket_fd)
protected:
	
private:
	
};