#include "../Hearthstone/CardCollection.h"
#include "../Hearthstone/Adept.h"
#include "../Hearthstone/Spell.h"
#include "../Hearthstone/Legend.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <filesystem>

void CardCollection::clearCards() {
    for (Card* c : cards) delete c;
    cards.clear();
}

CardCollection::CardCollection() {}

CardCollection::CardCollection(const CardCollection& other) {
    for (Card* c : other.cards)
        cards.push_back(c->clone());
}

CardCollection& CardCollection::operator=(const CardCollection& other) {
    if (this != &other) {
        clearCards();
        for (Card* c : other.cards)
            cards.push_back(c->clone());
    }
    return *this;
}

CardCollection::~CardCollection() { clearCards(); }

void CardCollection::loadFromFiles(const std::string& dataFolder) {
    clearCards();

    auto openFile = [](std::ifstream& f, const std::string& dataFolder, const std::string& name) {
        f.open(dataFolder + "/" + name);
        if (!f) {
            f.clear();
            f.open(name);
        }
        if (!f) {
            f.clear();
            f.open("../" + name);
        }
    };

    std::ifstream fa;
    openFile(fa, dataFolder, "adepts.txt");
    if (fa) {
        int count; fa >> count; fa.ignore();
        for (int i = 0; i < count; i++) {
            std::string name; int cost, atk, hp;
            std::getline(fa >> std::ws, name);
            fa >> cost >> atk >> hp; fa.ignore();
            cards.push_back(new Adept(name, cost, atk, hp));
        }
    }

    std::ifstream fs;
    openFile(fs, dataFolder, "spells.txt");
    if (fs) {
        int count; fs >> count; fs.ignore();
        for (int i = 0; i < count; i++) {
            std::string name, effStr; int cost, val;
            std::getline(fs >> std::ws, name);
            fs >> cost; fs.ignore();
            std::getline(fs >> std::ws, effStr);
            fs >> val; fs.ignore();
            cards.push_back(new Spell(name, cost, spellEffectFromString(effStr), val));
        }
    }

    std::ifstream fc;
    openFile(fc, dataFolder, "legends.txt");
    if (fc) {
        int count; fc >> count; fc.ignore();
        for (int i = 0; i < count; i++) {
            std::string name, effStr, title; int cost, atk, hp, val;
            std::getline(fc >> std::ws, name);
            fc >> cost >> atk >> hp; fc.ignore();
            std::getline(fc >> std::ws, effStr);
            fc >> val; fc.ignore();
            std::getline(fc >> std::ws, title);
            cards.push_back(new Legend(name, cost, atk, hp, spellEffectFromString(effStr), val, title));
        }
    }
}

void CardCollection::saveToFiles(const std::string& dataFolder) const {
    std::filesystem::create_directories(dataFolder);
    std::ofstream fa(dataFolder + "/adepts.txt");
    std::ofstream fs(dataFolder + "/spells.txt");
    std::ofstream fc(dataFolder + "/legends.txt");

    int ca = 0, cs = 0, cc = 0;
    for (Card* c : cards) {
        if (c->getType() == "Adept") ca++;
        else if (c->getType() == "Spell") cs++;
        else if (c->getType() == "Legend") cc++;
    }
    fa << ca << "\n"; fs << cs << "\n"; fc << cc << "\n";

    for (Card* c : cards) {
        if (c->getType() == "Adept") {
            Adept* a = dynamic_cast<Adept*>(c);
            fa << a->getName() << "\n" << a->getManaCost() << " "
               << a->getAttack() << " " << a->getMaxHealth() << "\n";
        } else if (c->getType() == "Spell") {
            Spell* s = dynamic_cast<Spell*>(c);
            fs << s->getName() << "\n" << s->getManaCost() << "\n"
               << spellEffectToString(s->getEffect()) << "\n" << s->getValue() << "\n";
        } else if (c->getType() == "Legend") {
            Legend* ch = dynamic_cast<Legend*>(c);
            fc << ch->getName() << "\n" << ch->getManaCost() << " "
               << ch->getAttack() << " " << ch->getMaxHealth() << "\n"
               << spellEffectToString(ch->getEffect()) << "\n" << ch->getValue() << "\n"
               << ch->getTitle() << "\n";
        }
    }
}

int CardCollection::getCount() const { return static_cast<int>(cards.size()); }

Card* CardCollection::getCard(int index) const {
    if (index < 0 || index >= static_cast<int>(cards.size()))
        throw std::out_of_range("Invalid card index");
    return cards[index];
}

Card* CardCollection::findByName(const std::string& name) const {
    for (Card* c : cards)
        if (c->getName() == name) return c;
    return nullptr;
}

void CardCollection::addCard(Card* card) {
    if (findByName(card->getName()) != nullptr)
        throw std::runtime_error("Card already exists: " + card->getName());
    cards.push_back(card->clone());
}

void CardCollection::removeCard(int index) {
    if (index < 0 || index >= static_cast<int>(cards.size()))
        throw std::out_of_range("Invalid card index");
    delete cards[index];
    cards.erase(cards.begin() + index);
}

std::ostream& operator<<(std::ostream& os, const CardCollection& col) {
    os << "=== Card Collection (" << col.cards.size() << " carti) ===\n";
    for (int i = 0; i < static_cast<int>(col.cards.size()); i++)
        os << "  " << (i + 1) << ". " << *col.cards[i] << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, CardCollection& col) {
    std::string type;
    std::cout << "Card type (Adept/Spell/Legend): ";
    is >> type;
    Card* card = nullptr;
    if (type == "Adept") {
        Adept* a = new Adept();
        is >> *a;
        card = a;
    } else if (type == "Spell") {
        Spell* s = new Spell();
        is >> *s;
        card = s;
    } else if (type == "Legend") {
        Legend* ch = new Legend();
        is >> *ch;
        card = ch;
    } else {
        throw std::invalid_argument("Unknown card type: " + type);
    }
    try {
        col.addCard(card);
        delete card;
    } catch (...) {
        delete card;
        throw;
    }
    return is;
}