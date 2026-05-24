#include "../Hearthstone/Spell.h"
#include "../Hearthstone/Exceptions.h"
#include <iostream>

std::string spellEffectToString(SpellEffect effect) {
    switch (effect) {
        case SpellEffect::DAMAGE:      return "DAMAGE";
        case SpellEffect::HEAL:        return "HEAL";
        case SpellEffect::BUFF_ATTACK: return "BUFF_ATTACK";
        case SpellEffect::BUFF_HEALTH: return "BUFF_HEALTH";
        case SpellEffect::BUFF_BOTH:   return "BUFF_BOTH";
        default:                       return "UNKNOWN";
    }
}

SpellEffect spellEffectFromString(const std::string& str) {
    if (str == "DAMAGE")      return SpellEffect::DAMAGE;
    if (str == "HEAL")        return SpellEffect::HEAL;
    if (str == "BUFF_ATTACK") return SpellEffect::BUFF_ATTACK;
    if (str == "BUFF_HEALTH") return SpellEffect::BUFF_HEALTH;
    if (str == "BUFF_BOTH")   return SpellEffect::BUFF_BOTH;
    throw InvalidCardTypeException("SpellEffect: " + str);
}

void Spell::setEffect(SpellEffect e) { effect = e; }

void Spell::setValue(int v) {
    if (v <= 0) throw HearthstoneException("Spell value must be positive");
    value = v;
}

Spell::Spell() : Card(), effect(SpellEffect::DAMAGE), value(1) {}

Spell::Spell(const std::string& name, int manaCost, SpellEffect eff, int val)
    : Card(name, manaCost) {
    setEffect(eff);
    setValue(val);
}

SpellEffect Spell::getEffect() const { return effect; }
int Spell::getValue() const { return value; }

void Spell::play() {
    std::cout << "  >> Spell [" << getName() << "] cast! Efect: "
              << spellEffectToString(effect) << " (" << value << ")\n";
}

std::string Spell::getType() const { return "Spell"; }
Card* Spell::clone() const { return new Spell(*this); }

void Spell::display(std::ostream& os) const {
    Card::display(os);
    os << " Effect:" << spellEffectToString(effect) << " Val:" << value;
}

void Spell::read(std::istream& is) {
    Card::read(is);
    std::string eff; int v;
    std::cout << "  Effect (DAMAGE/HEAL/BUFF_ATTACK/BUFF_HEALTH/BUFF_BOTH): ";
    is >> eff;
    std::cout << "  Value: ";
    is >> v;
    setEffect(spellEffectFromString(eff));
    setValue(v);
}