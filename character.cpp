#include "character.h"

Character::Character() { this->p.x = this->p.y = this->p.z = 0; }

Character::Character(int x, int y, int z) {
	this->p.x = x;
	this->p.y = y;
	this->p.z = z;
}

void Character::set_position(int x, int y, int z) {
	this->p.x = x;
	this->p.y = y;
	this->p.z = z;
}

Position Character::get_position() {
	return p;
}

void Character::show_position() {
	std::cout << this->p.x << ' ' << this->p.y << ' ' << this->p.z << std::endl;
}

void Character::update_position(int x, int y, int z) {
	this->p.x += x;
	this->p.y += y;
	this->p.z += z;
}

/*
 * convert_to_buffer
 * Convert Position to char[5] for sending it via sendto()
 */
const char* Character::convert_to_buffer() {
	// Convert Position to char array for sending it via sendto()
	std::stringstream b;	// stringstream buffer
	b << this->p.x << ' ' << this->p.y << ' ' << this->p.z;
	return b.str().c_str();
}

/*
 * get_buffer_length
 * Get buffer length for sendto()
 */
int Character::get_buffer_length() {
	// Get buffer lenght for sendto()
	std::stringstream b;	// stringstream buffer
	b << this->p.x << ' ' << this->p.y << ' ' << this->p.z;
	return b.str().length();
}