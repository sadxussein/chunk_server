#include "player.h"

Player::Player() { this->p.x = this->p.y = this->p.z = 0; }

Player::Player(int x, int y, int z) {
	this->p.x = x;
	this->p.y = y;
	this->p.z = z;
}

void Player::set_position(int x, int y, int z) { 
	this->p.x = x;
	this->p.y = y;
	this->p.z = z;
}

Position Player::get_position() {
	return p;
}

void Player::show_position() {
	std::cout << this->p.x << ' ' << this->p.y << ' ' << this->p.z << std::endl;
}

void Player::update_position(int x, int y, int z) {
	this->p.x += x;
	this->p.y += y;
	this->p.z += z;
}

/*
 * convert_to_buffer
 * Convert Position to char[5] for sending it via sendto()
 */
const char* Player::convert_to_buffer() {
	// Convert Position to char array for sending it via sendto()
	std::stringstream b;	// stringstream buffer
	b << this->p.x << ' ' << this->p.y << ' ' << this->p.z;
	return b.str().c_str();
}

/*
 * get_buffer_length
 * Get buffer lenght for sendto()
 */
int Player::get_buffer_length() {
	// Get buffer lenght for sendto()
	std::stringstream b;	// stringstream buffer
	b << this->p.x << ' ' << this->p.y << ' ' << this->p.z;
	return b.str().length();
}