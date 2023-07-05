#include <iostream>
#include <sstream>

struct Position {
	int x, y, z;
};


class Player {
public:
	Player();
	Player(int x, int y, int z);
	Position get_position();
	void set_position(int x, int y, int z);
	void update_position(int x, int y, int z);
	void show_position();
	const char* convert_to_buffer();
	int get_buffer_length();
private:
	Position p;
};