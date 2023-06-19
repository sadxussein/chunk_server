#include "main.h"

class chunk_server {
public:	
	chunk_server();
	int init_socket(int type, int port);
	vector<int>* get_sockets();
	~chunk_server();
private:
	vector<int> sockets;
};