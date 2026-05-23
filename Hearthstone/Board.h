#pragma once
#include "Adept.h"
#include "Player.h"
#include <vector>
#include <string>
#include <iostream>

class Board {
    std::vector<Adept*> minions[2];
    static const int MAX_MINIONS = 7;

    void clearSide(int side);
    void copySide(int side, const std::vector<Adept*>& src);
    void printSeparator(int width) const;

public:
    Board();
    Board(const Board& other);
    Board& operator=(const Board& other);
    ~Board();

    int getMinionCount(int side) const;
    Adept* getMinion(int side, int index) const;
    bool isFull(int side) const;

    void addMinion(int side, Adept* minion);
    void removeDeadMinions();
    void clear();

    void attackMinionWithMinion(int attackerSide, int attackerIdx, int targetIdx);
    void attackPlayerWithMinion(int attackerSide, int attackerIdx, Player& targetPlayer);

    void display(const Player& hero0, const Player& hero1) const;

    friend std::ostream& operator<<(std::ostream& os, const Board& board);
    friend std::istream& operator>>(std::istream& is, Board& board);
};