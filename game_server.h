#include "server.h"

class Game_server: public Server {
public:
	void handle_tcp_client(int client_tcp_socket_fd) override;
	void handle_udp_client() override;
protected:
	
private:
	
};