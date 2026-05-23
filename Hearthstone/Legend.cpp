#include "../Hearthstone/Legend.h"
#include <stdexcept>
#include <iostream>

void Legend::setTitle(const std::string& t) {
    if (t.empty()) throw std::invalid_argument("Legend title cannot be empty");
    title = t;
}

Legend::Legend()
    : Card(), Adept(), Spell(), title("Unknown Legend") {}

Legend::Legend(const std::string& name, int manaCost,
                   int attack, int health,
                   SpellEffect spellEffect, int spellValue,
                   const std::string& t)
    : Card(name, manaCost),
      Adept(name, manaCost, attack, health),
      Spell(name, manaCost, spellEffect, spellValue),
      title(t) {}

Legend::Legend(const Legend& other)
    : Card(other), Adept(other), Spell(other), title(other.title) {}

Legend& Legend::operator=(const Legend& other) {
    if (this != &other) {
        Adept::operator=(other);
        Spell::operator=(other);
        title = other.title;
    }
    return *this;
}

Legend::~Legend() {}

std::string Legend::getTitle() const { return title; }

void Legend::play() {
    std::cout << "  >> Legend [" << getName() << " - " << title << "] intra pe tabla!\n";
    std::cout << "     Stats: " << getAttack() << "/" << getCurrentHealth() << "\n";
    std::cout << "     Spell activat: " << spellEffectToString(getEffect())
              << " (" << getValue() << ")\n";
}

std::string Legend::getType() const { return "Legend"; }

Card* Legend::clone() const { return new Legend(*this); }

void Legend::display(std::ostream& os) const {
    Card::display(os);
    os << " [" << title << "] ATK:" << getAttack()
       << " HP:" << getCurrentHealth() << "/" << getMaxHealth()
       << " Spell:" << spellEffectToString(getEffect()) << "(" << getValue() << ")";
}

void Legend::read(std::istream& is) {
    Card::read(is);
    int atk, hp;
    std::cout << "  Attack: ";
    is >> atk;
    std::cout << "  Health: ";
    is >> hp;
    setAttack(atk);
    setMaxHealth(hp);

    std::string eff;
    int v;
    std::cout << "  Spell Effect (DAMAGE/HEAL/BUFF_ATTACK/BUFF_HEALTH/BUFF_BOTH): ";
    is >> eff;
    std::cout << "  Spell Value: ";
    is >> v;
    setEffect(spellEffectFromString(eff));
    setValue(v);

    std::cout << "  Title: ";
    std::getline(is >> std::ws, title);
}