#include "../Hearthstone/GameEngine.h"
#include "../Hearthstone/Legend.h"
#include <stdexcept>
#include <iostream>

void GameEngine::clearHands() {
    for (int s = 0; s < 2; s++) {
        for (Card* c : hands[s]) delete c;
        hands[s].clear();
    }
}

void GameEngine::drawCard(int side) {
    if (decks[side]->isEmpty()) {
        std::cout << "  [INFO] " << players[side]->getName() << " nu mai are carti in deck!\n";
        players[side]->takeDamage(1);
        return;
    }
    if (static_cast<int>(hands[side].size()) >= MAX_HAND_SIZE) {
        Card* burned = decks[side]->drawCard();
        std::cout << "  [INFO] Mana plina! Carte " << burned->getName() << " a fost arsa.\n";
        delete burned;
        return;
    }
    hands[side].push_back(decks[side]->drawCard());
}

void GameEngine::dealStartingHands() {
    for (int s = 0; s < 2; s++)
        for (int i = 0; i < STARTING_HAND_SIZE; i++)
            drawCard(s);
}

void GameEngine::incrementMana(int side) {
    if (currentMana[side] < maxMana[side]) currentMana[side]++;
}

void GameEngine::restoreMana(int side) {
    currentMana[side] = maxMana[side];
}

void GameEngine::applyBuffToMinions(int side, SpellEffect effect, int value) {
    bool buffed = false;
    for (int i = 0; i < board->getMinionCount(side); i++) {
        Adept* a = board->getMinion(side, i);
        if (effect == SpellEffect::BUFF_ATTACK || effect == SpellEffect::BUFF_BOTH)
            a->buffAttack(value);
        if (effect == SpellEffect::BUFF_HEALTH || effect == SpellEffect::BUFF_BOTH)
            a->buffHealth(value);
        buffed = true;
    }
    if (!buffed)
        std::cout << "  [INFO] Nu ai minioni pe tabla de buffat.\n";
}

void GameEngine::applyDamageSpell(int casterSide, int value) {
    int enemySide = 1 - casterSide;
    std::cout << "  Tinteste: (0) Player adversar";
    for (int i = 0; i < board->getMinionCount(enemySide); i++)
        std::cout << " | (" << (i + 1) << ") " << board->getMinion(enemySide, i)->getName();
    std::cout << "\n  Alegere: ";
    int choice;
    std::cin >> choice;
    if (choice == 0) {
        players[enemySide]->takeDamage(value);
        std::cout << "  " << players[enemySide]->getName() << " primeste " << value << " damage!\n";
    } else {
        int idx = choice - 1;
        if (idx >= 0 && idx < board->getMinionCount(enemySide)) {
            board->getMinion(enemySide, idx)->takeDamage(value);
            std::cout << "  Minion lovit!\n";
            board->removeDeadMinions();
        } else {
            std::cout << "  Tinta invalida, spell pierdut.\n";
        }
    }
}

void GameEngine::applyHealSpell(int casterSide, int value) {
    std::cout << "  Tinteste: (0) Player tau";
    for (int i = 0; i < board->getMinionCount(casterSide); i++)
        std::cout << " | (" << (i + 1) << ") " << board->getMinion(casterSide, i)->getName();
    std::cout << "\n  Alegere: ";
    int choice;
    std::cin >> choice;
    if (choice == 0) {
        players[casterSide]->heal(value);
        std::cout << "  Player healed!\n";
    } else {
        int idx = choice - 1;
        if (idx >= 0 && idx < board->getMinionCount(casterSide)) {
            board->getMinion(casterSide, idx)->healToMax();
            std::cout << "  Minion healed la max HP!\n";
        } else {
            std::cout << "  Tinta invalida, spell pierdut.\n";
        }
    }
}

void GameEngine::applySpellEffect(Spell* spell, int casterSide) {
    SpellEffect effect = spell->getEffect();
    int value = spell->getValue();
    if (effect == SpellEffect::DAMAGE)
        applyDamageSpell(casterSide, value);
    else if (effect == SpellEffect::HEAL)
        applyHealSpell(casterSide, value);
    else
        applyBuffToMinions(casterSide, effect, value);
}

void GameEngine::printHeader() const {
    std::cout << "\n";
    std::cout << "\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n";
    std::cout << "\xBA           HEARTHSTONE ASCII  -  Tura " << (currentTurn + 1)
              << "             \xBA\n";
    std::cout << "\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n";
}

void GameEngine::printHand(int side) const {
    std::cout << "  Carti in mana (" << hands[side].size() << " carti):\n";
    for (int i = 0; i < static_cast<int>(hands[side].size()); i++)
        std::cout << "    [" << i << "] " << *hands[side][i] << "\n";
}

void GameEngine::printMana(int side) const {
    std::cout << "  Mana: " << currentMana[side] << "/" << maxMana[side] << "  |  ";
}

GameEngine::GameEngine()
    : board(nullptr), currentTurn(0), gameOver(false), winner(-1) {
    for (int s = 0; s < 2; s++) {
        players[s] = nullptr;
        decks[s] = nullptr;
        currentMana[s] = 0;
        maxMana[s] = 10;
    }
}

GameEngine::GameEngine(User* user0, Deck* deck0, User* user1, Deck* deck1)
    : currentTurn(0), gameOver(false), winner(-1) {
    players[0] = new Player(user0->getUsername());
    players[1] = new Player(user1->getUsername());
    decks[0] = new Deck(*deck0);
    decks[1] = new Deck(*deck1);
    decks[0]->shuffle();
    decks[1]->shuffle();
    currentMana[0] = 0; currentMana[1] = 0;
    maxMana[0] = 10;    maxMana[1] = 10;
    board = new Board();
}

GameEngine::GameEngine(const GameEngine& other)
    : currentTurn(other.currentTurn), gameOver(other.gameOver), winner(other.winner) {
    for (int s = 0; s < 2; s++) {
        players[s] = new Player(*other.players[s]);
        decks[s]   = new Deck(*other.decks[s]);
        currentMana[s] = other.currentMana[s];
        maxMana[s]     = other.maxMana[s];
        for (Card* c : other.hands[s]) hands[s].push_back(c->clone());
    }
    board = new Board(*other.board);
}

GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        clearHands();
        for (int s = 0; s < 2; s++) {
            delete players[s]; delete decks[s];
            players[s] = new Player(*other.players[s]);
            decks[s]   = new Deck(*other.decks[s]);
            currentMana[s] = other.currentMana[s];
            maxMana[s]     = other.maxMana[s];
            for (Card* c : other.hands[s]) hands[s].push_back(c->clone());
        }
        delete board;
        board = new Board(*other.board);
        currentTurn = other.currentTurn;
        gameOver    = other.gameOver;
        winner      = other.winner;
    }
    return *this;
}

GameEngine::~GameEngine() {
    clearHands();
    for (int s = 0; s < 2; s++) { delete players[s]; delete decks[s]; }
    delete board;
}

bool GameEngine::isGameOver() const { return gameOver; }
int  GameEngine::getWinner()  const { return winner; }

void GameEngine::startGame() {
    gameOver = false;
    winner   = -1;
    currentTurn = 0;
    board->clear();
    clearHands();
    dealStartingHands();
    std::cout << "\n  *** Jocul incepe! Noroc amandurora! ***\n";
    processTurn();
}

void GameEngine::processTurn() {
    while (!gameOver) {
        int side = currentTurn % 2;
        incrementMana(side);
        drawCard(side);
        checkWinCondition();
        if (gameOver) break;

        printHeader();
        board->display(*players[0], *players[1]);
        printMana(side);
        std::cout << players[side]->getName() << " joaca.\n";
        printHand(side);

        bool turnEnded = false;
        while (!turnEnded && !gameOver) {
            std::cout << "\n  [1] Joaca o carte  [2] Ataca cu minion  [3] Termina tura  [0] Abandon\n";
            std::cout << "  > ";
            int opt; std::cin >> opt;
            switch (opt) {
                case 1: {
                    printHand(side);
                    std::cout << "  Index carte: ";
                    int idx; std::cin >> idx;
                    try { playCardFromHand(side, idx); }
                    catch (const std::exception& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
                    checkWinCondition();
                    break;
                }
                case 2: {
                    if (board->getMinionCount(side) == 0) {
                        std::cout << "  [INFO] Nu ai minioni pe tabla.\n"; break;
                    }
                    std::cout << "  Minionii tai:\n";
                    for (int i = 0; i < board->getMinionCount(side); i++)
                        std::cout << "    [" << i << "] " << *board->getMinion(side, i) << "\n";
                    std::cout << "  Index minion atacator: ";
                    int atk; std::cin >> atk;
                    std::cout << "  Tinteste: (0) Player adversar";
                    for (int i = 0; i < board->getMinionCount(1 - side); i++)
                        std::cout << " | (" << (i + 1) << ") " << board->getMinion(1 - side, i)->getName();
                    std::cout << "\n  > ";
                    int tgt; std::cin >> tgt;
                    try {
                        if (tgt == 0) attackWithMinion(side, atk, 0, true);
                        else          attackWithMinion(side, atk, tgt - 1, false);
                    } catch (const std::exception& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
                    checkWinCondition();
                    break;
                }
                case 3:
                    endTurn();
                    turnEnded = true;
                    break;
                case 0:
                    std::cout << "  " << players[side]->getName() << " a abandonat!\n";
                    winner   = 1 - side;
                    gameOver = true;
                    turnEnded = true;
                    break;
                default:
                    std::cout << "  [ERR] Optiune invalida.\n";
            }
        }
        if (!gameOver) currentTurn++;
    }
    std::cout << "\n\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n";
    if (winner >= 0)
        std::cout << "\xBA  CASTIGATOR: " << players[winner]->getName()
                  << std::string(39 - players[winner]->getName().size(), ' ') << "\xBA\n";
    else
        std::cout << "\xBA  Egalitate!                                          \xBA\n";
    std::cout << "\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n";
}

void GameEngine::playCardFromHand(int side, int handIndex) {
    if (handIndex < 0 || handIndex >= static_cast<int>(hands[side].size()))
        throw std::out_of_range("Index carte invalid");
    Card* card = hands[side][handIndex];
    if (card->getManaCost() > currentMana[side])
        throw std::runtime_error("Mana insuficienta! Cost: " + std::to_string(card->getManaCost())
                                 + ", ai: " + std::to_string(currentMana[side]));

    if ((card->getType() == "Adept" || card->getType() == "Legend") && board->isFull(side)) {
        throw std::runtime_error("Board plin pe partea ta (max 7)!");
    }

    currentMana[side] -= card->getManaCost();
    card->play();

    if (card->getType() == "Adept") {
        board->addMinion(side, dynamic_cast<Adept*>(card));
    } else if (card->getType() == "Spell") {
        applySpellEffect(dynamic_cast<Spell*>(card), side);
    } else if (card->getType() == "Legend") {
        Legend* ch = dynamic_cast<Legend*>(card);
        board->addMinion(side, ch);
        applySpellEffect(ch, side);
    }

    delete hands[side][handIndex];
    hands[side].erase(hands[side].begin() + handIndex);
}

void GameEngine::attackWithMinion(int side, int minionIndex, int targetIndex, bool targetPlayer) {
    if (targetPlayer)
        board->attackPlayerWithMinion(side, minionIndex, *players[1 - side]);
    else
        board->attackMinionWithMinion(side, minionIndex, targetIndex);
}

void GameEngine::endTurn() {
    std::cout << "  -- Tura terminata. --\n";
}

void GameEngine::checkWinCondition() {
    if (!players[0]->isAlive() && !players[1]->isAlive()) {
        gameOver = true; winner = -1;
    } else if (!players[0]->isAlive()) {
        gameOver = true; winner = 1;
    } else if (!players[1]->isAlive()) {
        gameOver = true; winner = 0;
    }
}

std::ostream& operator<<(std::ostream& os, const GameEngine& ge) {
    os << "GameEngine | Tura: " << ge.currentTurn
       << " | GameOver: " << (ge.gameOver ? "DA" : "NU") << "\n";
    if (ge.players[0]) os << "  P1: " << *ge.players[0] << "\n";
    if (ge.players[1]) os << "  P2: " << *ge.players[1] << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, GameEngine& ge) {
    (void)ge;
    return is;
}