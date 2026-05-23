#pragma once
#include "Deck.h"
#include <vector>
#include <string>

class User {
    std::string username;
    std::vector<Deck*> decks;

    void clearDecks();
    void copyDecks(const std::vector<Deck*>& src);

public:
    User();
    explicit User(const std::string& username);
    User(const User& other);
    User& operator=(const User& other);
    ~User();

    std::string getUsername() const;
    void setUsername(const std::string& name);

    int getDeckCount() const;
    Deck* getDeck(int index) const;
    Deck* getDeckByName(const std::string& name) const;

    void addDeck(const std::string& deckName);
    void removeDeck(int index);

    void saveToFile(const std::string& folder) const;
    void loadFromFile(const std::string& folder);

    friend std::ostream& operator<<(std::ostream& os, const User& user);
    friend std::istream& operator>>(std::istream& is, User& user);
};