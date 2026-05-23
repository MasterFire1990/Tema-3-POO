#pragma once
#include <string>
#include <iostream>

class Card {
    std::string name;
    int manaCost;

protected:
    void setName(const std::string& n);
    void setManaCost(int cost);

public:
    Card();
    Card(const std::string& name, int manaCost);
    Card(const Card& other);
    Card& operator=(const Card& other);
    virtual ~Card();

    std::string getName() const;
    int getManaCost() const;

    virtual void play() = 0;
    virtual std::string getType() const = 0;
    virtual Card* clone() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
    friend std::istream& operator>>(std::istream& is, Card& card);

    virtual void display(std::ostream& os) const;
    virtual void read(std::istream& is);
};