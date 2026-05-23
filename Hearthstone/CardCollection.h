#pragma once
#include "Card.h"
#include <vector>
#include <string>

class CardCollection {
    std::vector<Card*> cards;

    void clearCards();

public:
    CardCollection();
    CardCollection(const CardCollection& other);
    CardCollection& operator=(const CardCollection& other);
    ~CardCollection();

    void loadFromFiles(const std::string& dataFolder);
    void saveToFiles(const std::string& dataFolder) const;

    int getCount() const;
    Card* getCard(int index) const;
    Card* findByName(const std::string& name) const;

    void addCard(Card* card);
    void removeCard(int index);

    friend std::ostream& operator<<(std::ostream& os, const CardCollection& col);
    friend std::istream& operator>>(std::istream& is, CardCollection& col);
};