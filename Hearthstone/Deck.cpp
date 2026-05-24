#include "../Hearthstone/Deck.h"
#include "../Hearthstone/CardFactory.h"
#include "../Hearthstone/Adept.h"
#include "../Hearthstone/Spell.h"
#include "../Hearthstone/Legend.h"
#include "../Hearthstone/Exceptions.h"
#include <fstream>
#include <iostream>

Deck::Deck() : deckName("Unnamed"), ownerUsername("Unknown") {}

Deck::Deck(const std::string& name, const std::string& owner)
    : deckName(name), ownerUsername(owner) {}

Deck::Deck(const Deck& other)
    : deckName(other.deckName), ownerUsername(other.ownerUsername) {
    for (const auto& c : other.cards)
        cards.push_back(std::unique_ptr<Card>(c->clone()));
}

Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        deckName = other.deckName;
        ownerUsername = other.ownerUsername;
        cards.clear();
        for (const auto& c : other.cards)
            cards.push_back(std::unique_ptr<Card>(c->clone()));
    }
    return *this;
}

std::string Deck::getDeckName() const { return deckName; }
std::string Deck::getOwnerUsername() const { return ownerUsername; }
int Deck::getSize() const { return static_cast<int>(cards.size()); }
bool Deck::isEmpty() const { return cards.empty(); }
bool Deck::isFull() const { return static_cast<int>(cards.size()) >= MAX_CARDS; }

void Deck::addCard(const Card& card) {
    if (isFull()) throw DeckFullException();
    cards.push_back(std::unique_ptr<Card>(card.clone()));
}

void Deck::removeCard(int index) {
    if (index < 0 || index >= static_cast<int>(cards.size()))
        throw InvalidIndexException("Deck::removeCard");
    cards.erase(cards.begin() + index);
}

std::unique_ptr<Card> Deck::drawCard() {
    if (isEmpty()) throw HearthstoneException("Deck is empty");
    auto top = std::move(cards.back());
    cards.pop_back();
    return top;
}

void Deck::shuffle() {
    std::mt19937 rng(std::random_device{}());
    std::shuffle(cards.begin(), cards.end(), rng);
}

void Deck::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) throw HearthstoneException("Cannot open file: " + filename);
    out << deckName << "\n" << ownerUsername << "\n" << cards.size() << "\n";
    for (const auto& c : cards) {
        out << c->getType() << "\n" << c->getName() << "\n" << c->getManaCost() << "\n";
        if (c->getType() == "Adept") {
            const auto* a = dynamic_cast<const Adept*>(c.get());
            out << a->getAttack() << "\n" << a->getMaxHealth() << "\n";
        } else if (c->getType() == "Spell") {
            const auto* s = dynamic_cast<const Spell*>(c.get());
            out << spellEffectToString(s->getEffect()) << "\n" << s->getValue() << "\n";
        } else if (c->getType() == "Legend") {
            const auto* ch = dynamic_cast<const Legend*>(c.get());
            out << ch->getAttack() << "\n" << ch->getMaxHealth() << "\n"
                << spellEffectToString(ch->getEffect()) << "\n"
                << ch->getValue() << "\n" << ch->getTitle() << "\n";
        }
    }
}

void Deck::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw HearthstoneException("Cannot open file: " + filename);
    cards.clear();
    std::getline(in, deckName);
    std::getline(in, ownerUsername);
    int count; in >> count; in.ignore();
    for (int i = 0; i < count; i++) {
        std::string type, name, effStr;
        int cost, val;
        std::getline(in, type);
        std::getline(in, name);
        in >> cost; in.ignore();
        int atk = 0, hp = 0;
        if (type == "Adept") {
            in >> atk >> hp; in.ignore();
        } else if (type == "Spell") {
            std::getline(in, effStr);
            in >> val; in.ignore();
        } else if (type == "Legend") {
            in >> atk >> hp; in.ignore();
            std::getline(in, effStr);
            in >> val; in.ignore();
            std::string ttl; std::getline(in, ttl);
            cards.push_back(CardFactory::create(type, name, cost, atk, hp, effStr, val, ttl));
            continue;
        }
        cards.push_back(CardFactory::create(type, name, cost, atk, hp, effStr, val));
    }
}

std::ostream& operator<<(std::ostream& os, const Deck& deck) {
    os << "Deck: " << deck.deckName << " [" << deck.ownerUsername
       << "] (" << deck.cards.size() << "/30 carti)\n";
    for (int i = 0; i < static_cast<int>(deck.cards.size()); i++)
        os << "  " << (i + 1) << ". " << *deck.cards[i] << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Deck& deck) {
    std::cout << "Deck name: ";
    std::getline(is >> std::ws, deck.deckName);
    return is;
}