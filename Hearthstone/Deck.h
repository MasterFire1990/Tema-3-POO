#pragma once
#include "Card.h"
#include <vector>
#include <string>
#include <algorithm>
#include <random>

class Deck {
    std::string deckName;
    std::string ownerUsername;
    std::vector<Card*> cards;

    static const int MAX_CARDS = 30;

    void clearCards();
    void copyCards(const std::vector<Card*>& src);

public:
    Deck();
    Deck(const std::string& deckName, const std::string& ownerUsername);
    Deck(const Deck& other);
    Deck& operator=(const Deck& other);
    ~Deck();

    std::string getDeckName() const;
    std::string getOwnerUsername() const;
    int getSize() const;
    bool isEmpty() const;
    bool isFull() const;

    void addCard(Card* card);
    void removeCard(int index);
    Card* drawCard();
    void shuffle();

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

    friend std::ostream& operator<<(std::ostream& os, const Deck& deck);
    friend std::istream& operator>>(std::istream& is, Deck& deck);
};