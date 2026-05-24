#include "../Hearthstone/CardCollection.h"
#include "../Hearthstone/CardFactory.h"
#include "../Hearthstone/Adept.h"
#include "../Hearthstone/Spell.h"
#include "../Hearthstone/Legend.h"
#include "../Hearthstone/Exceptions.h"
#include <fstream>
#include <iostream>
#include <filesystem>

CardCollection::CardCollection(const CardCollection& other) {
    cards.cloneFrom(other.cards);
}

CardCollection& CardCollection::operator=(const CardCollection& other) {
    if (this != &other)
        cards.cloneFrom(other.cards);
    return *this;
}

void CardCollection::loadFromFiles(const std::string& dataFolder) {
    cards.clear();

    auto tryOpen = [](const std::string& folder, const std::string& file) {
        std::ifstream f(folder + "/" + file);
        if (!f) f.open(file);
        return f;
    };

    if (auto fa = tryOpen(dataFolder, "adepts.txt"); fa) {
        int count; fa >> count; fa.ignore();
        for (int i = 0; i < count; i++) {
            std::string name; int cost, atk, hp;
            std::getline(fa >> std::ws, name);
            fa >> cost >> atk >> hp; fa.ignore();
            cards.add(CardFactory::create("Adept", name, cost, atk, hp));
        }
    }

    if (auto fs = tryOpen(dataFolder, "spells.txt"); fs) {
        int count; fs >> count; fs.ignore();
        for (int i = 0; i < count; i++) {
            std::string name, effStr; int cost, val;
            std::getline(fs >> std::ws, name);
            fs >> cost; fs.ignore();
            std::getline(fs >> std::ws, effStr);
            fs >> val; fs.ignore();
            cards.add(CardFactory::create("Spell", name, cost, 0, 0, effStr, val));
        }
    }

    if (auto fc = tryOpen(dataFolder, "legends.txt"); fc) {
        int count; fc >> count; fc.ignore();
        for (int i = 0; i < count; i++) {
            std::string name, effStr, title; int cost, atk, hp, val;
            std::getline(fc >> std::ws, name);
            fc >> cost >> atk >> hp; fc.ignore();
            std::getline(fc >> std::ws, effStr);
            fc >> val; fc.ignore();
            std::getline(fc >> std::ws, title);
            cards.add(CardFactory::create("Legend", name, cost, atk, hp, effStr, val, title));
        }
    }
}

void CardCollection::saveToFiles(const std::string& dataFolder) const {
    std::filesystem::create_directories(dataFolder);
    std::ofstream fa(dataFolder + "/adepts.txt");
    std::ofstream fs(dataFolder + "/spells.txt");
    std::ofstream fc(dataFolder + "/legends.txt");

    int ca = 0, cs = 0, cl = 0;
    for (int i = 0; i < cards.size(); i++) {
        const auto* c = cards.get(i);
        if (c->getType() == "Adept") ca++;
        else if (c->getType() == "Spell") cs++;
        else if (c->getType() == "Legend") cl++;
    }
    fa << ca << "\n"; fs << cs << "\n"; fc << cl << "\n";

    for (int i = 0; i < cards.size(); i++) {
        const auto* c = cards.get(i);
        if (c->getType() == "Adept") {
            const auto* a = dynamic_cast<const Adept*>(c);
            fa << a->getName() << "\n" << a->getManaCost() << " "
               << a->getAttack() << " " << a->getMaxHealth() << "\n";
        } else if (c->getType() == "Spell") {
            const auto* s = dynamic_cast<const Spell*>(c);
            fs << s->getName() << "\n" << s->getManaCost() << "\n"
               << spellEffectToString(s->getEffect()) << "\n" << s->getValue() << "\n";
        } else if (c->getType() == "Legend") {
            const auto* ch = dynamic_cast<const Legend*>(c);
            fc << ch->getName() << "\n" << ch->getManaCost() << " "
               << ch->getAttack() << " " << ch->getMaxHealth() << "\n"
               << spellEffectToString(ch->getEffect()) << "\n" << ch->getValue() << "\n"
               << ch->getTitle() << "\n";
        }
    }
}

int CardCollection::getCount() const { return cards.size(); }

Card* CardCollection::getCard(int index) const { return cards.get(index); }

Card* CardCollection::findByName(const std::string& name) const {
    return ::findByName(cards, name);
}

void CardCollection::addCard(const Card& card) {
    if (findByName(card.getName()))
        throw DuplicateCardException(card.getName());
    cards.add(std::unique_ptr<Card>(card.clone()));
}

void CardCollection::removeCard(int index) { cards.remove(index); }

const Repository<Card>& CardCollection::getRepository() const { return cards; }

std::ostream& operator<<(std::ostream& os, const CardCollection& col) {
    os << "=== Card Collection (" << col.cards.size() << " carti) ===\n";
    displayAll(col.cards, os);
    return os;
}

std::istream& operator>>(std::istream& is, CardCollection& col) {
    std::string type;
    std::cout << "Card type (Adept/Spell/Legend): ";
    is >> type;
    auto card = CardFactory::createEmpty(type);
    is >> *card;
    try {
        col.addCard(*card);
    } catch (...) { throw; }
    return is;
}