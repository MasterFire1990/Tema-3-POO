#include "../Hearthstone/Deck.h"
#include "../Hearthstone/Adept.h"
#include "../Hearthstone/Spell.h"
#include "../Hearthstone/Legend.h"
#include <stdexcept>
#include <fstream>
#include <iostream>

void Deck::clearCards() {
    for (Card* c : cards) delete c;
    cards.clear();
}

void Deck::copyCards(const std::vector<Card*>& src) {
    for (Card* c : src)
        cards.push_back(c->clone());
}

Deck::Deck() : deckName("Unnamed"), ownerUsername("Unknown") {}

Deck::Deck(const std::string& name, const std::string& owner)
    : deckName(name), ownerUsername(owner) {}

Deck::Deck(const Deck& other)
    : deckName(other.deckName), ownerUsername(other.ownerUsername) {
    copyCards(other.cards);
}

Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        clearCards();
        deckName = other.deckName;
        ownerUsername = other.ownerUsername;
        copyCards(other.cards);
    }
    return *this;
}

Deck::~Deck() { clearCards(); }

std::string Deck::getDeckName() const { return deckName; }
std::string Deck::getOwnerUsername() const { return ownerUsername; }
int Deck::getSize() const { return static_cast<int>(cards.size()); }
bool Deck::isEmpty() const { return cards.empty(); }
bool Deck::isFull() const { return static_cast<int>(cards.size()) >= MAX_CARDS; }

void Deck::addCard(Card* card) {
    if (isFull()) throw std::runtime_error("Deck is full (max 30 cards)");
    cards.push_back(card->clone());
}

void Deck::removeCard(int index) {
    if (index < 0 || index >= static_cast<int>(cards.size()))
        throw std::out_of_range("Invalid card index");
    delete cards[index];
    cards.erase(cards.begin() + index);
}

Card* Deck::drawCard() {
    if (isEmpty()) throw std::runtime_error("Deck is empty");
    Card* top = cards.back();
    cards.pop_back();
    return top;
}

void Deck::shuffle() {
    std::mt19937 rng(std::random_device{}());
    std::shuffle(cards.begin(), cards.end(), rng);
}

void Deck::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Cannot open file: " + filename);
    out << deckName << "\n" << ownerUsername << "\n" << cards.size() << "\n";
    for (const Card* c : cards) {
        out << c->getType() << "\n";
        out << c->getName() << "\n";
        out << c->getManaCost() << "\n";
        if (c->getType() == "Adept") {
            const Adept* a = dynamic_cast<const Adept*>(c);
            out << a->getAttack() << "\n" << a->getMaxHealth() << "\n";
        } else if (c->getType() == "Spell") {
            const Spell* s = dynamic_cast<const Spell*>(c);
            out << spellEffectToString(s->getEffect()) << "\n" << s->getValue() << "\n";
        } else if (c->getType() == "Legend") {
            const Legend* ch = dynamic_cast<const Legend*>(c);
            out << ch->getAttack() << "\n" << ch->getMaxHealth() << "\n";
            out << spellEffectToString(ch->getEffect()) << "\n" << ch->getValue() << "\n";
            out << ch->getTitle() << "\n";
        }
    }
}

void Deck::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Cannot open file: " + filename);
    clearCards();
    std::getline(in, deckName);
    std::getline(in, ownerUsername);
    int count;
    in >> count;
    in.ignore();
    for (int i = 0; i < count; i++) {
        std::string type, name, effStr;
        int cost, val;
        std::getline(in, type);
        std::getline(in, name);
        in >> cost;
        in.ignore();
        if (type == "Adept") {
            int atk, hp;
            in >> atk >> hp;
            in.ignore();
            cards.push_back(new Adept(name, cost, atk, hp));
        } else if (type == "Spell") {
            std::getline(in, effStr);
            in >> val;
            in.ignore();
            cards.push_back(new Spell(name, cost, spellEffectFromString(effStr), val));
        } else if (type == "Legend") {
            int atk, hp;
            in >> atk >> hp;
            in.ignore();
            std::getline(in, effStr);
            in >> val;
            in.ignore();
            std::string ttl;
            std::getline(in, ttl);
            cards.push_back(new Legend(name, cost, atk, hp, spellEffectFromString(effStr), val, ttl));
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Deck& deck) {
    os << "Deck: " << deck.deckName << " [" << deck.ownerUsername << "] ("
       << deck.cards.size() << "/30 carti)\n";
    for (int i = 0; i < static_cast<int>(deck.cards.size()); i++)
        os << "  " << (i + 1) << ". " << *deck.cards[i] << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Deck& deck) {
    std::cout << "Deck name: ";
    std::getline(is >> std::ws, deck.deckName);
    return is;
}