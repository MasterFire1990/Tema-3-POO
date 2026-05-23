#pragma once
#include "Card.h"

class Adept : public virtual Card {
    int attack;
    int maxHealth;
    int currentHealth;

protected:
    void setAttack(int atk);
    void setMaxHealth(int hp);

public:
    Adept();
    Adept(const std::string& name, int manaCost, int attack, int health);
    Adept(const Adept& other);
    Adept& operator=(const Adept& other);
    ~Adept() override;

    int getAttack() const;
    int getCurrentHealth() const;
    int getMaxHealth() const;

    void takeDamage(int amount);
    void healToMax();
    void buffAttack(int amount);
    void buffHealth(int amount);
    bool isAlive() const;

    void play() override;
    std::string getType() const override;
    Card* clone() const override;

    void display(std::ostream& os) const override;
    void read(std::istream& is) override;
};