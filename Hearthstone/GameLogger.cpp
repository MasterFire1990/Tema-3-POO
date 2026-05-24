#include "../Hearthstone/GameLogger.h"
#include <chrono>
#include <ctime>
#include <stdexcept>

GameLogger::GameLogger(const std::string& filename) {
    logFile.open(filename, std::ios::app);
    if (!logFile) throw std::runtime_error("Nu pot deschide fisierul de log: " + filename);
    log("=== Sesiune noua ===");
}

GameLogger::~GameLogger() {
    if (logFile.is_open()) {
        log("=== Sesiune incheiata ===");
        logFile.close();
    }
}

GameLogger& GameLogger::getInstance(const std::string& filename) {
    static GameLogger instance(filename);
    return instance;
}

void GameLogger::log(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
    logFile << "[" << buf << "] " << message << "\n";
    logFile.flush();
}

void GameLogger::onMinionDied(const std::string& name) {
    log("MORT: " + name);
}

void GameLogger::onPlayerDamaged(const std::string& name, int amount) {
    log("DAMAGE: " + name + " primeste " + std::to_string(amount) + " damage");
}

void GameLogger::onMinionPlayed(const std::string& name) {
    log("PLAY: Minion [" + name + "] pus pe tabla");
}

void GameLogger::onSpellCast(const std::string& name) {
    log("SPELL: [" + name + "] cast");
}

void GameLogger::onGameOver(const std::string& winnerName) {
    log("GAME OVER: Castigator -> " + winnerName);
}

void GameLogger::onTurnStart(const std::string& playerName, int turn) {
    log("TURA " + std::to_string(turn) + ": " + playerName + " joaca");
}