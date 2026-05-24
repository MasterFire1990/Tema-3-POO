#pragma once
#include "IGameObserver.h"
#include <fstream>
#include <string>

class GameLogger : public IGameObserver {
    std::ofstream logFile;

    explicit GameLogger(const std::string& filename);

public:
    GameLogger(const GameLogger&) = delete;
    GameLogger& operator=(const GameLogger&) = delete;
    GameLogger(GameLogger&&) = delete;
    GameLogger& operator=(GameLogger&&) = delete;
    ~GameLogger() override;

    static GameLogger& getInstance(const std::string& filename = "game_log.txt");

    void onMinionDied(const std::string& name) override;
    void onPlayerDamaged(const std::string& name, int amount) override;
    void onMinionPlayed(const std::string& name) override;
    void onSpellCast(const std::string& name) override;
    void onGameOver(const std::string& winnerName) override;
    void onTurnStart(const std::string& playerName, int turn) override;

private:
    void log(const std::string& message);
};