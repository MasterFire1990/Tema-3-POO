#include "../Hearthstone/GameEngine.h"
#include "../Hearthstone/Legend.h"
#include "../Hearthstone/Exceptions.h"
#include <iostream>
#include <functional>

void GameEngine::notifyObservers(std::function<void(IGameObserver&)> fn) {
    for (auto& weak : observers) {
        if (auto obs = weak.lock()) fn(*obs);
    }
}

void GameEngine::clearHands() {
    for (int s = 0; s < 2; s++) hands[s].clear();
}

void GameEngine::drawCard(int side) {
    if (decks[side]->isEmpty()) {
        players[side]->takeDamage(1);
        notifyObservers([&](IGameObserver& o) {
            o.onPlayerDamaged(players[side]->getName(), 1);
        });
        return;
    }
    if (static_cast<int>(hands[side].size()) >= MAX_HAND_SIZE) {
        auto burned = decks[side]->drawCard();
        std::cout << "  [INFO] Mana plina! Carte " << burned->getName() << " arsa.\n";
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
    if (!buffed) std::cout << "  [INFO] Nu ai minioni de buffat.\n";
}

void GameEngine::applyDamageSpell(int casterSide, int value) {
    int enemySide = 1 - casterSide;
    std::cout << "  Tinteste: (0) Player adversar";
    for (int i = 0; i < board->getMinionCount(enemySide); i++)
        std::cout << " | (" << (i + 1) << ") " << board->getMinion(enemySide, i)->getName();
    std::cout << "\n  Alegere: ";
    int choice; std::cin >> choice;
    if (choice == 0) {
        players[enemySide]->takeDamage(value);
        notifyObservers([&](IGameObserver& o) {
            o.onPlayerDamaged(players[enemySide]->getName(), value);
        });
        std::cout << "  " << players[enemySide]->getName() << " primeste " << value << " damage!\n";
    } else {
        int idx = choice - 1;
        if (idx >= 0 && idx < board->getMinionCount(enemySide)) {
            board->getMinion(enemySide, idx)->takeDamage(value);
            board->removeDeadMinions(observers.empty() ? nullptr :
                observers[0].lock().get());
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
    int choice; std::cin >> choice;
    if (choice == 0) {
        players[casterSide]->heal(value);
    } else {
        int idx = choice - 1;
        if (idx >= 0 && idx < board->getMinionCount(casterSide))
            board->getMinion(casterSide, idx)->healToMax();
        else
            std::cout << "  Tinta invalida, spell pierdut.\n";
    }
}

void GameEngine::applySpellEffect(Spell* spell, int casterSide) {
    notifyObservers([&](IGameObserver& o) { o.onSpellCast(spell->getName()); });
    switch (spell->getEffect()) {
        case SpellEffect::DAMAGE: applyDamageSpell(casterSide, spell->getValue()); break;
        case SpellEffect::HEAL:   applyHealSpell(casterSide, spell->getValue()); break;
        default: applyBuffToMinions(casterSide, spell->getEffect(), spell->getValue()); break;
    }
}

void GameEngine::printHeader() const {
    std::cout << "\n";
    std::cout << "========================================================\n";
    std::cout << "       HEARTHSTONE ASCII  -  Tura " << (currentTurn + 1) << "\n";
    std::cout << "========================================================\n";
}

void GameEngine::printHand(int side) const {
    std::cout << "  Carti in mana (" << hands[side].size() << "):\n";
    for (int i = 0; i < static_cast<int>(hands[side].size()); i++)
        std::cout << "    [" << i << "] " << *hands[side][i] << "\n";
}

void GameEngine::printMana(int side) const {
    std::cout << "  Mana: " << currentMana[side] << "/" << maxMana[side] << "  |  ";
}

GameEngine::GameEngine()
    : board(nullptr), currentTurn(0), gameOver(false), winner(-1) {
    for (int s = 0; s < 2; s++) {
        players[s] = nullptr; decks[s] = nullptr;
        currentMana[s] = 0;   maxMana[s] = 10;
    }
}

GameEngine::GameEngine(User* user0, Deck* deck0, User* user1, Deck* deck1)
    : currentTurn(0), gameOver(false), winner(-1) {
    players[0] = std::make_unique<Player>(user0->getName());
    players[1] = std::make_unique<Player>(user1->getName());
    decks[0] = std::make_unique<Deck>(*deck0);
    decks[1] = std::make_unique<Deck>(*deck1);
    decks[0]->shuffle(); decks[1]->shuffle();
    currentMana[0] = 0; currentMana[1] = 0;
    maxMana[0] = 10;    maxMana[1] = 10;
    board = std::make_unique<Board>();
}

GameEngine::GameEngine(const GameEngine& other)
    : currentTurn(other.currentTurn), gameOver(other.gameOver), winner(other.winner) {
    for (int s = 0; s < 2; s++) {
        if (other.players[s]) players[s] = std::make_unique<Player>(*other.players[s]);
        if (other.decks[s])   decks[s]   = std::make_unique<Deck>(*other.decks[s]);
        currentMana[s] = other.currentMana[s];
        maxMana[s]     = other.maxMana[s];
        for (const auto& c : other.hands[s])
            hands[s].push_back(std::unique_ptr<Card>(c->clone()));
    }
    if (other.board) board = std::make_unique<Board>(*other.board);
}

GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        clearHands();
        for (int s = 0; s < 2; s++) {
            players[s] = other.players[s] ? std::make_unique<Player>(*other.players[s]) : nullptr;
            decks[s]   = other.decks[s]   ? std::make_unique<Deck>(*other.decks[s])     : nullptr;
            currentMana[s] = other.currentMana[s];
            maxMana[s]     = other.maxMana[s];
            for (const auto& c : other.hands[s])
                hands[s].push_back(std::unique_ptr<Card>(c->clone()));
        }
        board       = other.board ? std::make_unique<Board>(*other.board) : nullptr;
        currentTurn = other.currentTurn;
        gameOver    = other.gameOver;
        winner      = other.winner;
    }
    return *this;
}

void GameEngine::addObserver(std::weak_ptr<IGameObserver> obs) {
    observers.push_back(obs);
}

bool GameEngine::isGameOver() const { return gameOver; }
int  GameEngine::getWinner()  const { return winner; }

void GameEngine::startGame() {
    gameOver = false; winner = -1; currentTurn = 0;
    board->clear(); clearHands(); dealStartingHands();
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

        notifyObservers([&](IGameObserver& o) {
            o.onTurnStart(players[side]->getName(), currentTurn + 1);
        });

        printHeader();
        board->display(*players[0], *players[1]);
        printMana(side);
        std::cout << players[side]->getName() << " joaca.\n";
        printHand(side);

        bool turnEnded = false;
        while (!turnEnded && !gameOver) {
            std::cout << "\n  [1] Joaca o carte  [2] Ataca cu minion  [3] Termina tura  [0] Abandon\n  > ";
            int opt; std::cin >> opt;
            switch (opt) {
                case 1: {
                    printHand(side);
                    std::cout << "  Index carte: ";
                    int idx; std::cin >> idx;
                    try { playCardFromHand(side, idx); }
                    catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
                    checkWinCondition();
                    break;
                }
                case 2: {
                    if (board->getMinionCount(side) == 0) {
                        std::cout << "  [INFO] Nu ai minioni pe tabla.\n"; break;
                    }
                    for (int i = 0; i < board->getMinionCount(side); i++)
                        std::cout << "    [" << i << "] " << *board->getMinion(side, i) << "\n";
                    std::cout << "  Index minion atacator: ";
                    int atk; std::cin >> atk;
                    std::cout << "  Tinteste: (0) Player adversar";
                    for (int i = 0; i < board->getMinionCount(1 - side); i++)
                        std::cout << " | (" << (i+1) << ") " << board->getMinion(1-side,i)->getName();
                    std::cout << "\n  > ";
                    int tgt; std::cin >> tgt;
                    try {
                        if (tgt == 0) attackWithMinion(side, atk, 0, true);
                        else          attackWithMinion(side, atk, tgt - 1, false);
                    } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
                    checkWinCondition();
                    break;
                }
                case 3: endTurn(); turnEnded = true; break;
                case 0:
                    std::cout << "  " << players[side]->getName() << " a abandonat!\n";
                    winner = 1 - side; gameOver = true; turnEnded = true; break;
                default: std::cout << "  [ERR] Optiune invalida.\n";
            }
        }
        if (!gameOver) currentTurn++;
    }
    std::string winnerName = winner >= 0 ? players[winner]->getName() : "Egalitate";
    notifyObservers([&](IGameObserver& o) { o.onGameOver(winnerName); });
    std::cout << "\n========================================================\n";
    if (winner >= 0)
        std::cout << "  CASTIGATOR: " << players[winner]->getName() << "\n";
    else
        std::cout << "  Egalitate!\n";
    std::cout << "========================================================\n";
}

void GameEngine::playCardFromHand(int side, int handIndex) {
    if (handIndex < 0 || handIndex >= static_cast<int>(hands[side].size()))
        throw InvalidIndexException("playCardFromHand");
    Card* card = hands[side][handIndex].get();
    if (card->getManaCost() > currentMana[side])
        throw InsufficientManaException(card->getManaCost(), currentMana[side]);
    if ((card->getType() == "Adept" || card->getType() == "Legend") && board->isFull(side))
        throw BoardFullException();

    currentMana[side] -= card->getManaCost();
    card->play();

    if (card->getType() == "Adept") {
        board->addMinion(side, *dynamic_cast<Adept*>(card));
        notifyObservers([&](IGameObserver& o) { o.onMinionPlayed(card->getName()); });
    } else if (card->getType() == "Spell") {
        applySpellEffect(dynamic_cast<Spell*>(card), side);
    } else if (card->getType() == "Legend") {
        auto* ch = dynamic_cast<Legend*>(card);
        board->addMinion(side, *ch);
        notifyObservers([&](IGameObserver& o) { o.onMinionPlayed(card->getName()); });
        applySpellEffect(ch, side);
    }
    hands[side].erase(hands[side].begin() + handIndex);
}

void GameEngine::attackWithMinion(int side, int minionIndex, int targetIndex, bool targetPlayer) {
    IGameObserver* obs = observers.empty() ? nullptr : observers[0].lock().get();
    if (targetPlayer)
        board->attackPlayerWithMinion(side, minionIndex, *players[1 - side], obs);
    else
        board->attackMinionWithMinion(side, minionIndex, targetIndex, obs);
}

void GameEngine::endTurn() { std::cout << "  -- Tura terminata. --\n"; }

void GameEngine::checkWinCondition() {
    if (!players[0]->isAlive() && !players[1]->isAlive()) { gameOver = true; winner = -1; }
    else if (!players[0]->isAlive()) { gameOver = true; winner = 1; }
    else if (!players[1]->isAlive()) { gameOver = true; winner = 0; }
}

std::ostream& operator<<(std::ostream& os, const GameEngine& ge) {
    os << "GameEngine | Tura: " << ge.currentTurn
       << " | GameOver: " << (ge.gameOver ? "DA" : "NU") << "\n";
    if (ge.players[0]) os << "  P1: " << *ge.players[0] << "\n";
    if (ge.players[1]) os << "  P2: " << *ge.players[1] << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, GameEngine& ge) { (void)ge; return is; }