#pragma once
#include "Card.h"
#include <string>

enum class SpellEffect { DAMAGE, HEAL, BUFF_ATTACK, BUFF_HEALTH, BUFF_BOTH };

std::string spellEffectToString(SpellEffect effect);
SpellEffect spellEffectFromString(const std::string& str);

class Spell : public virtual Card {
    SpellEffect effect;
    int value;

protected:
    void setEffect(SpellEffect e);
    void setValue(int v);

public:
    Spell();
    Spell(const std::string& name, int manaCost, SpellEffect effect, int value);
    Spell(const Spell& other);
    Spell& operator=(const Spell& other);
    ~Spell() override;

    SpellEffect getEffect() const;
    int getValue() const;

    void play() override;
    std::string getType() const override;
    Card* clone() const override;

    void display(std::ostream& os) const override;
    void read(std::istream& is) override;
};