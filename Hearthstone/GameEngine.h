#pragma once
#include "Board.h"
#include "Player.h"
#include "Deck.h"
#include "User.h"
#include "CardCollection.h"
#include "Spell.h"
#include <vector>
#include <string>

class GameEngine {
    Player* players[2];
    Deck* decks[2];
    std::vector<Card*> hands[2];
    Board* board;
    int currentMana[2];
    int maxMana[2];
    int currentTurn;
    bool gameOver;
    int winner;

    static const int MAX_HAND_SIZE = 10;
    static const int STARTING_HAND_SIZE = 3;

    void clearHands();
    void drawCard(int side);
    void dealStartingHands();
    void incrementMana(int side);
    void restoreMana(int side);

    void applySpellEffect(Spell* spell, int casterSide);
    void applyBuffToMinions(int side, SpellEffect effect, int value);
    void applyDamageSpell(int casterSide, int value);
    void applyHealSpell(int casterSide, int value);

    void printHeader() const;
    void printHand(int side) const;
    void printMana(int side) const;

public:
    GameEngine();
    GameEngine(User* user0, Deck* deck0, User* user1, Deck* deck1);
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);
    ~GameEngine();

    bool isGameOver() const;
    int getWinner() const;

    void startGame();
    void processTurn();
    void playCardFromHand(int side, int handIndex);
    void attackWithMinion(int side, int minionIndex, int targetIndex, bool targetPlayer);
    void endTurn();
    void checkWinCondition();

    friend std::ostream& operator<<(std::ostream& os, const GameEngine& ge);
    friend std::istream& operator>>(std::istream& is, GameEngine& ge);
};