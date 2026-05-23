#pragma once
#include "Adept.h"
#include "Spell.h"

class Legend : public Adept, public Spell {
    std::string title;

    void setTitle(const std::string& t);

public:
    Legend();
    Legend(const std::string& name, int manaCost,
             int attack, int health,
             SpellEffect spellEffect, int spellValue,
             const std::string& title);
    Legend(const Legend& other);
    Legend& operator=(const Legend& other);
    ~Legend() override;

    std::string getTitle() const;

    void play() override;
    std::string getType() const override;
    Card* clone() const override;

    void display(std::ostream& os) const override;
    void read(std::istream& is) override;
};