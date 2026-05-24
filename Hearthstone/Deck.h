#pragma once
#include "Card.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <random>

class Deck {
    std::string deckName;
    std::string ownerUsername;
    std::vector<std::unique_ptr<Card>> cards;

    static const int MAX_CARDS = 30;

public:
    Deck();
    Deck(const std::string& deckName, const std::string& ownerUsername);
    Deck(const Deck& other);
    Deck& operator=(const Deck& other);
    Deck(Deck&&) = default;
    Deck& operator=(Deck&&) = default;
    ~Deck() = default;

    std::string getDeckName() const;
    std::string getOwnerUsername() const;
    int getSize() const;
    bool isEmpty() const;
    bool isFull() const;

    void addCard(const Card& card);
    void removeCard(int index);
    std::unique_ptr<Card> drawCard();
    void shuffle();

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

    friend std::ostream& operator<<(std::ostream& os, const Deck& deck);
    friend std::istream& operator>>(std::istream& is, Deck& deck);
};