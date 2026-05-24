#pragma once
#include "Adept.h"
#include "Player.h"
#include <vector>
#include <memory>
#include <iostream>

class Board {
    std::vector<std::unique_ptr<Adept>> minions[2];
    static const int MAX_MINIONS = 7;

    void copySide(int side, const std::vector<std::unique_ptr<Adept>>& src);
    void printSeparator(int width) const;

public:
    Board() = default;
    Board(const Board& other);
    Board& operator=(const Board& other);
    Board(Board&&) = default;
    Board& operator=(Board&&) = default;
    ~Board() = default;

    int getMinionCount(int side) const;
    Adept* getMinion(int side, int index) const;
    bool isFull(int side) const;

    void addMinion(int side, const Adept& minion);
    void removeDeadMinions(class IGameObserver* observer = nullptr);
    void clear();

    void attackMinionWithMinion(int attackerSide, int attackerIdx, int targetIdx,
                                 class IGameObserver* observer = nullptr);
    void attackPlayerWithMinion(int attackerSide, int attackerIdx, Player& targetPlayer,
                                 class IGameObserver* observer = nullptr);

    void display(const Player& player0, const Player& player1) const;

    friend std::ostream& operator<<(std::ostream& os, const Board& board);
    friend std::istream& operator>>(std::istream& is, Board& board);
};