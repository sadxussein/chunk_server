#include <iostream>
#include <algorithm>
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

const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1500;

using namespace std;