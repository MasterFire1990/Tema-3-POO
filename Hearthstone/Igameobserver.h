#pragma once
#include <string>

class IGameObserver {
public:
    virtual ~IGameObserver() = default;
    virtual void onMinionDied(const std::string& name) = 0;
    virtual void onPlayerDamaged(const std::string& name, int amount) = 0;
    virtual void onMinionPlayed(const std::string& name) = 0;
    virtual void onSpellCast(const std::string& name) = 0;
    virtual void onGameOver(const std::string& winnerName) = 0;
    virtual void onTurnStart(const std::string& playerName, int turn) = 0;
};