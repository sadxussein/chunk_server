#include <iostream>
#include <sstream>

struct Position {
	int x, y, z;
};

class Character {
public:
	Character();
	Character(int x, int y, int z);
	Position get_position();
	void set_position(int x, int y, int z);
	void update_position(int x, int y, int z);
	void show_position();
	const char* convert_to_buffer();
	int get_buffer_length();
private:
	Position p;
    // table CHARACTERS fields
    size_t id;
    std::string name;
    size_t class_id;
    size_t race_id;
    size_t experience_points;
    size_t level;
    size_t current_health;
    size_t current_mana;
};