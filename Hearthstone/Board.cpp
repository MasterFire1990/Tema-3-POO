#include "../Hearthstone/Board.h"
#include <stdexcept>
#include <iomanip>

void Board::clearSide(int side) {
    for (Adept* a : minions[side]) delete a;
    minions[side].clear();
}

void Board::copySide(int side, const std::vector<Adept*>& src) {
    for (Adept* a : src)
        minions[side].push_back(dynamic_cast<Adept*>(a->clone()));
}

Board::Board() {}

Board::Board(const Board& other) {
    copySide(0, other.minions[0]);
    copySide(1, other.minions[1]);
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        clearSide(0); clearSide(1);
        copySide(0, other.minions[0]);
        copySide(1, other.minions[1]);
    }
    return *this;
}

Board::~Board() { clearSide(0); clearSide(1); }

int Board::getMinionCount(int side) const {
    return static_cast<int>(minions[side].size());
}

Adept* Board::getMinion(int side, int index) const {
    if (side < 0 || side > 1) throw std::out_of_range("Invalid side");
    if (index < 0 || index >= static_cast<int>(minions[side].size()))
        throw std::out_of_range("Invalid minion index");
    return minions[side][index];
}

bool Board::isFull(int side) const {
    return static_cast<int>(minions[side].size()) >= MAX_MINIONS;
}

void Board::addMinion(int side, Adept* minion) {
    if (isFull(side)) throw std::runtime_error("Board is full for this side (max 7)");
    minions[side].push_back(dynamic_cast<Adept*>(minion->clone()));
}

void Board::removeDeadMinions() {
    for (int side = 0; side < 2; side++) {
        for (int i = static_cast<int>(minions[side].size()) - 1; i >= 0; i--) {
            if (!minions[side][i]->isAlive()) {
                std::cout << "  [RIP] " << minions[side][i]->getName() << " a murit!\n";
                delete minions[side][i];
                minions[side].erase(minions[side].begin() + i);
            }
        }
    }
}

void Board::clear() { clearSide(0); clearSide(1); }

void Board::attackMinionWithMinion(int attackerSide, int attackerIdx, int targetIdx) {
    int defenderSide = 1 - attackerSide;
    Adept* attacker = getMinion(attackerSide, attackerIdx);
    Adept* defender = getMinion(defenderSide, targetIdx);
    std::cout << "  " << attacker->getName() << " (" << attacker->getAttack() << "/"
              << attacker->getCurrentHealth() << ") ataca "
              << defender->getName() << " (" << defender->getAttack() << "/"
              << defender->getCurrentHealth() << ")\n";
    attacker->takeDamage(defender->getAttack());
    defender->takeDamage(attacker->getAttack());
    removeDeadMinions();
}

void Board::attackPlayerWithMinion(int attackerSide, int attackerIdx, Player& targetPlayer) {
    Adept* attacker = getMinion(attackerSide, attackerIdx);
    std::cout << "  " << attacker->getName() << " ataca player-ul "
              << targetPlayer.getName() << " pentru " << attacker->getAttack() << " damage!\n";
    targetPlayer.takeDamage(attacker->getAttack());
}

void Board::printSeparator(int width) const {
    for (int i = 0; i < width; i++) std::cout << "-";
    std::cout << "\n";
}

void Board::display(const Player& player0, const Player& player1) const {
    printSeparator(60);
    std::cout << "  " << player1.getName() << " | HP: " << player1.getHealth() << "/" << player1.getMaxHealth() << "\n";
    std::cout << "  Tabla adversar: ";
    if (minions[1].empty()) std::cout << "(gol)";
    for (const Adept* a : minions[1])
        std::cout << "[" << a->getName() << " " << a->getAttack() << "/" << a->getCurrentHealth() << "] ";
    std::cout << "\n";
    printSeparator(60);
    std::cout << "  Tabla ta: ";
    if (minions[0].empty()) std::cout << "(gol)";
    for (const Adept* a : minions[0])
        std::cout << "[" << a->getName() << " " << a->getAttack() << "/" << a->getCurrentHealth() << "] ";
    std::cout << "\n";
    std::cout << "  " << player0.getName() << " | HP: " << player0.getHealth() << "/" << player0.getMaxHealth() << "\n";
    printSeparator(60);
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    os << "Board:\n";
    os << "  Player 1 (" << board.minions[0].size() << " minioni): ";
    for (const Adept* a : board.minions[0])
        os << a->getName() << " ";
    os << "\n  Player 2 (" << board.minions[1].size() << " minioni): ";
    for (const Adept* a : board.minions[1])
        os << a->getName() << " ";
    os << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Board& board) {
    (void)board;
    return is;
}