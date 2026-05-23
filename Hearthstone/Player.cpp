#include "../Hearthstone/Player.h"
#include <stdexcept>

void Player::setMaxHealth(int hp) {
    if (hp <= 0) throw std::invalid_argument("Player health must be positive");
    maxHealth = hp;
    health = hp;
}

Player::Player() : name("Unknown"), health(20), maxHealth(20) {}

Player::Player(const std::string& n) : name(n), health(20), maxHealth(20) {}

Player::Player(const Player& other)
    : name(other.name), health(other.health), maxHealth(other.maxHealth) {}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        name = other.name;
        health = other.health;
        maxHealth = other.maxHealth;
    }
    return *this;
}

Player::~Player() {}

std::string Player::getName() const { return name; }
int Player::getHealth() const { return health; }
int Player::getMaxHealth() const { return maxHealth; }
bool Player::isAlive() const { return health > 0; }

void Player::takeDamage(int amount) {
    if (amount < 0) return;
    health -= amount;
    if (health < 0) health = 0;
}

void Player::heal(int amount) {
    if (amount < 0) return;
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void Player::reset() {
    health = maxHealth;
}

std::ostream& operator<<(std::ostream& os, const Player& hero) {
    os << "Player: " << hero.name << " HP: " << hero.health << "/" << hero.maxHealth;
    return os;
}

std::istream& operator>>(std::istream& is, Player& hero) {
    std::cout << "Player name: ";
    std::getline(is >> std::ws, hero.name);
    return is;
}