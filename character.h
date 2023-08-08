#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <sstream>
#include <utility>

struct Position {
	float x, y, z;
};

class Character {
public:
	Character();
    Character(size_t id, std::string name, size_t class_id,
        size_t race_id, size_t experience_points, size_t level,
        size_t current_health, size_t current_mana);

	void show_position();
	const char* convert_to_buffer();
	int get_buffer_length();
    void update_position(float x, float y, float z);
    // setters and getters
    size_t getId() const;
    void setId(size_t id);
    const std::string &getName() const;
    void setName(const std::string &name);
    size_t getClassId() const;
    void setClassId(size_t classId);
    size_t getRaceId() const;
    void setRaceId(size_t raceId);
    size_t getExperiencePoints() const;
    void setExperiencePoints(size_t experiencePoints);
    size_t getLevel() const;
    void setLevel(size_t level);
    size_t getCurrentHealth() const;
    void setCurrentHealth(size_t currentHealth);
    size_t getCurrentMana() const;
    void setCurrentMana(size_t currentMana);
    Position getPosition() const;
    void setPosition(float x, float y, float z);
private:
    // table CHARACTERS fields
    Position position;
    size_t id;
    std::string name;
    size_t class_id;

private:
    size_t race_id;
    size_t experience_points;
    size_t level;
    size_t current_health;
    size_t current_mana;
};

#endif // CHARACTER_H