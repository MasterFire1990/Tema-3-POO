#pragma once
#include "Card.h"
#include <memory>
#include <string>

class CardFactory {
public:
    // Creates a card from type string + raw attributes (used when loading from file)
    static std::unique_ptr<Card> create(
        const std::string& type,
        const std::string& name,
        int manaCost,
        int attack = 0,
        int health = 0,
        const std::string& spellEffect = "",
        int spellValue = 0,
        const std::string& title = ""
    );

    // Creates an empty card of given type for interactive input
    static std::unique_ptr<Card> createEmpty(const std::string& type);

private:
    CardFactory() = delete;
};