#include "../Hearthstone/Adept.h"
#include "../Hearthstone/Exceptions.h"
#include <iostream>

void Adept::setAttack(int atk) {
    if (atk < 0) throw HearthstoneException("Attack cannot be negative");
    attack = atk;
}

void Adept::setMaxHealth(int hp) {
    if (hp <= 0) throw HearthstoneException("Health must be positive");
    maxHealth = hp;
    currentHealth = hp;
}

Adept::Adept() : Card(), attack(1), maxHealth(1), currentHealth(1) {}

Adept::Adept(const std::string& name, int manaCost, int atk, int health)
    : Card(name, manaCost) {
    setAttack(atk);
    setMaxHealth(health);
}

int Adept::getAttack() const { return attack; }
int Adept::getCurrentHealth() const { return currentHealth; }
int Adept::getMaxHealth() const { return maxHealth; }
bool Adept::isAlive() const { return currentHealth > 0; }

void Adept::takeDamage(int amount) {
    if (amount < 0) return;
    currentHealth -= amount;
}

void Adept::healToMax() { currentHealth = maxHealth; }

void Adept::buffAttack(int amount) {
    if (amount < 0) return;
    attack += amount;
}

void Adept::buffHealth(int amount) {
    if (amount <= 0) return;
    maxHealth += amount;
    currentHealth += amount;
}

void Adept::play() {
    std::cout << "  >> Adept [" << getName() << "] intra pe tabla! ("
              << attack << "/" << currentHealth << ")\n";
}

std::string Adept::getType() const { return "Adept"; }
Card* Adept::clone() const { return new Adept(*this); }

void Adept::display(std::ostream& os) const {
    Card::display(os);
    os << " ATK:" << attack << " HP:" << currentHealth << "/" << maxHealth;
}

void Adept::read(std::istream& is) {
    Card::read(is);
    int atk, hp;
    std::cout << "  Attack: "; is >> atk;
    std::cout << "  Health: "; is >> hp;
    setAttack(atk);
    setMaxHealth(hp);
}