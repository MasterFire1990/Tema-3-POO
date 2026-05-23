#pragma once
#include <string>
#include <iostream>

class Player {
    std::string name;
    int health;
    int maxHealth;

    void setMaxHealth(int hp);

public:
    Player();
    explicit Player(const std::string& name);
    Player(const Player& other);
    Player& operator=(const Player& other);
    ~Player();

    std::string getName() const;
    int getHealth() const;
    int getMaxHealth() const;
    bool isAlive() const;

    void takeDamage(int amount);
    void heal(int amount);
    void reset();

    friend std::ostream& operator<<(std::ostream& os, const Player& hero);
    friend std::istream& operator>>(std::istream& is, Player& hero);
};