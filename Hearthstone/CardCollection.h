#pragma once
#include "Repository.h"
#include "Card.h"
#include <string>

class CardCollection {
    Repository<Card> cards;

public:
    CardCollection() = default;
    CardCollection(const CardCollection& other);
    CardCollection& operator=(const CardCollection& other);
    CardCollection(CardCollection&&) = default;
    CardCollection& operator=(CardCollection&&) = default;
    ~CardCollection() = default;

    void loadFromFiles(const std::string& dataFolder);
    void saveToFiles(const std::string& dataFolder) const;

    int getCount() const;
    Card* getCard(int index) const;
    Card* findByName(const std::string& name) const;

    void addCard(const Card& card);
    void removeCard(int index);

    const Repository<Card>& getRepository() const;

    friend std::ostream& operator<<(std::ostream& os, const CardCollection& col);
    friend std::istream& operator>>(std::istream& is, CardCollection& col);
};