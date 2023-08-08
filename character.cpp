#include "character.h"

Character::Character() {}

Character::Character(size_t id, std::string name, size_t class_id, size_t race_id,
                     size_t experience_points, size_t level, size_t current_health, size_t current_mana) {
    this->id = id; this->name = std::move(name); this->class_id = class_id; this->race_id = race_id;
    this->experience_points = experience_points; this->level = level;
    this->current_health = current_health; this->current_mana = current_mana;
}

void Character::show_position() {
	std::cout << this->position.x << ' ' << this->position.y << ' ' << this->position.z << std::endl;
}

void Character::update_position(float x, float y, float z) {
	this->position.x += x;
	this->position.y += y;
	this->position.z += z;
}

/*
 * convert_to_buffer
 * Convert Position to char[5] for sending it via sendto()
 */
const char* Character::convert_to_buffer() {
	// Convert Position to char array for sending it via sendto()
	std::stringstream b;	// stringstream buffer
	b << this->position.x << ' ' << this->position.y << ' ' << this->position.z;
	return b.str().c_str();
}

/*
 * get_buffer_length
 * Get buffer length for sendto()
 */
int Character::get_buffer_length() {
	// Get buffer lenght for sendto()
	std::stringstream b;	// stringstream buffer
	b << this->position.x << ' ' << this->position.y << ' ' << this->position.z;
	return b.str().length();
}

/*
 * bunch of setters and getters for character
 */
size_t Character::getId() const {
    return id;
}

void Character::setId(size_t id) {
    Character::id = id;
}

const std::string &Character::getName() const {
    return name;
}

void Character::setName(const std::string &name) {
    Character::name = name;
}

size_t Character::getClassId() const {
    return class_id;
}

void Character::setClassId(size_t classId) {
    class_id = classId;
}

size_t Character::getRaceId() const {
    return race_id;
}

void Character::setRaceId(size_t raceId) {
    race_id = raceId;
}

size_t Character::getExperiencePoints() const {
    return experience_points;
}

void Character::setExperiencePoints(size_t experiencePoints) {
    experience_points = experiencePoints;
}

size_t Character::getLevel() const {
    return level;
}

void Character::setLevel(size_t level) {
    Character::level = level;
}

size_t Character::getCurrentHealth() const {
    return current_health;
}

void Character::setCurrentHealth(size_t currentHealth) {
    current_health = currentHealth;
}

size_t Character::getCurrentMana() const {
    return current_mana;
}

void Character::setCurrentMana(size_t currentMana) {
    current_mana = currentMana;
}

void Character::setPosition(float x, float y, float z) {
    this->position.x = x;
    this->position.y = y;
    this->position.z = z;
}

Position Character::getPosition() const {
    return position;
}
/*
 * end of setters and getters for character
 */


