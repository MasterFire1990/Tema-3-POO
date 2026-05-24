#include "../Hearthstone/CardFactory.h"
#include "../Hearthstone/Adept.h"
#include "../Hearthstone/Spell.h"
#include "../Hearthstone/Legend.h"
#include "../Hearthstone/Exceptions.h"

std::unique_ptr<Card> CardFactory::create(
    const std::string& type,
    const std::string& name,
    int manaCost,
    int attack,
    int health,
    const std::string& spellEffect,
    int spellValue,
    const std::string& title)
{
    if (type == "Adept")
        return std::make_unique<Adept>(name, manaCost, attack, health);

    if (type == "Spell")
        return std::make_unique<Spell>(name, manaCost,
            spellEffectFromString(spellEffect), spellValue);

    if (type == "Legend")
        return std::make_unique<Legend>(name, manaCost, attack, health,
            spellEffectFromString(spellEffect), spellValue, title);

    throw InvalidCardTypeException(type);
}

std::unique_ptr<Card> CardFactory::createEmpty(const std::string& type) {
    if (type == "Adept")   return std::make_unique<Adept>();
    if (type == "Spell")   return std::make_unique<Spell>();
    if (type == "Legend")  return std::make_unique<Legend>();
    throw InvalidCardTypeException(type);
}