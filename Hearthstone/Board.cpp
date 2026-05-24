#include "../Hearthstone/Board.h"
#include "../Hearthstone/IGameObserver.h"
#include "../Hearthstone/Exceptions.h"

void Board::copySide(int side, const std::vector<std::unique_ptr<Adept>>& src) {
    for (const auto& a : src)
        minions[side].push_back(std::unique_ptr<Adept>(
            dynamic_cast<Adept*>(a->clone())));
}

Board::Board(const Board& other) {
    copySide(0, other.minions[0]);
    copySide(1, other.minions[1]);
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        minions[0].clear(); minions[1].clear();
        copySide(0, other.minions[0]);
        copySide(1, other.minions[1]);
    }
    return *this;
}

int Board::getMinionCount(int side) const {
    return static_cast<int>(minions[side].size());
}

Adept* Board::getMinion(int side, int index) const {
    if (side < 0 || side > 1 || index < 0 || index >= getMinionCount(side))
        throw InvalidIndexException("Board::getMinion");
    return minions[side][index].get();
}

bool Board::isFull(int side) const {
    return getMinionCount(side) >= MAX_MINIONS;
}

void Board::addMinion(int side, const Adept& minion) {
    if (isFull(side)) throw BoardFullException();
    minions[side].push_back(std::unique_ptr<Adept>(
        dynamic_cast<Adept*>(minion.clone())));
}

void Board::removeDeadMinions(IGameObserver* observer) {
    for (int side = 0; side < 2; side++) {
        for (int i = static_cast<int>(minions[side].size()) - 1; i >= 0; i--) {
            if (!minions[side][i]->isAlive()) {
                if (observer) observer->onMinionDied(minions[side][i]->getName());
                std::cout << "  [RIP] " << minions[side][i]->getName() << " a murit!\n";
                minions[side].erase(minions[side].begin() + i);
            }
        }
    }
}

void Board::clear() { minions[0].clear(); minions[1].clear(); }

void Board::attackMinionWithMinion(int attackerSide, int attackerIdx, int targetIdx,
                                    IGameObserver* observer) {
    Adept* attacker = getMinion(attackerSide, attackerIdx);
    Adept* defender = getMinion(1 - attackerSide, targetIdx);
    std::cout << "  " << attacker->getName() << " (" << attacker->getAttack() << "/"
              << attacker->getCurrentHealth() << ") ataca "
              << defender->getName() << " (" << defender->getAttack() << "/"
              << defender->getCurrentHealth() << ")\n";
    attacker->takeDamage(defender->getAttack());
    defender->takeDamage(attacker->getAttack());
    removeDeadMinions(observer);
}

void Board::attackPlayerWithMinion(int attackerSide, int attackerIdx, Player& targetPlayer,
                                    IGameObserver* observer) {
    Adept* attacker = getMinion(attackerSide, attackerIdx);
    std::cout << "  " << attacker->getName() << " ataca player-ul "
              << targetPlayer.getName() << " pentru " << attacker->getAttack() << " damage!\n";
    targetPlayer.takeDamage(attacker->getAttack());
    if (observer) observer->onPlayerDamaged(targetPlayer.getName(), attacker->getAttack());
}

void Board::printSeparator(int width) const {
    for (int i = 0; i < width; i++) std::cout << "-";
    std::cout << "\n";
}

void Board::display(const Player& p0, const Player& p1) const {
    printSeparator(60);
    std::cout << "  " << p1.getName() << " | HP: " << p1.getHealth() << "/" << p1.getMaxHealth() << "\n";
    std::cout << "  Tabla adversar: ";
    if (minions[1].empty()) std::cout << "(gol)";
    for (const auto& a : minions[1])
        std::cout << "[" << a->getName() << " " << a->getAttack() << "/" << a->getCurrentHealth() << "] ";
    std::cout << "\n";
    printSeparator(60);
    std::cout << "  Tabla ta: ";
    if (minions[0].empty()) std::cout << "(gol)";
    for (const auto& a : minions[0])
        std::cout << "[" << a->getName() << " " << a->getAttack() << "/" << a->getCurrentHealth() << "] ";
    std::cout << "\n";
    std::cout << "  " << p0.getName() << " | HP: " << p0.getHealth() << "/" << p0.getMaxHealth() << "\n";
    printSeparator(60);
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    os << "Board:\n";
    for (int side = 0; side < 2; side++) {
        os << "  Player " << (side + 1) << " (" << board.minions[side].size() << " minioni): ";
        for (const auto& a : board.minions[side]) os << a->getName() << " ";
        os << "\n";
    }
    return os;
}

std::istream& operator>>(std::istream& is, Board& board) {
    (void)board; return is;
}