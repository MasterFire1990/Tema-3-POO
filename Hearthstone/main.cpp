#include "Menu.h"
#include "Repository.h"
#include "CardFactory.h"
#include "Player.h"
#include "Adept.h"
#include "Spell.h"
#include "Legend.h"
#include "Exceptions.h"
#include "GameLogger.h"
#include <iostream>
#include <memory>



static void demonstrateRepositoryWithCards() {
    std::cout << "\n=== Repository<Card> ===\n";
    Repository<Card> cardRepo;

    cardRepo.add(CardFactory::create("Adept",  "Soldatul",  2, 2, 3));
    cardRepo.add(CardFactory::create("Spell",  "Sageata",   2, 0, 0, "DAMAGE", 3));
    cardRepo.add(CardFactory::create("Adept",  "Magul",     3, 3, 2));
    cardRepo.add(CardFactory::create("Legend", "Ragnaros",  8, 8, 8, "DAMAGE", 4, "Stapanul_Focului"));

    std::cout << "Toate cartile:\n";
    displayAll(cardRepo, std::cout);

    
    Card* found = findByName(cardRepo, "Magul");
    if (found) std::cout << "Gasit: " << *found << "\n";

    
    int adeptCount = countIf<Card>(cardRepo,
        [](const Card& c) { return c.getType() == "Adept"; });
    std::cout << "Numar adepti: " << adeptCount << "\n";

    
    cardRepo.sortBy([](const Card& a, const Card& b) {
        return a.getManaCost() < b.getManaCost();
    });
    std::cout << "Sortate dupa mana:\n";
    displayAll(cardRepo, std::cout);
}

static void demonstrateRepositoryWithPlayers() {
    std::cout << "\n=== Repository<Player> ===\n";
    Repository<Player> playerRepo;

    playerRepo.add(std::make_unique<Player>("Alice"));
    playerRepo.add(std::make_unique<Player>("Bob"));
    playerRepo.add(std::make_unique<Player>("Charlie"));

    std::cout << "Toti playerii:\n";
    displayAll(playerRepo, std::cout);

    
    Player* found = findByName(playerRepo, "Bob");
    if (found) {
        found->takeDamage(5);
        std::cout << "Bob dupa damage: " << *found << "\n";
    }

    
    int alive = countIf<Player>(playerRepo,
        [](const Player& p) { return p.isAlive(); });
    std::cout << "Playeri in viata: " << alive << "\n";
}

static void demonstratePolymorphism() {
    std::cout << "\n=== Polimorfism prin vector<unique_ptr<Card>> ===\n";
    std::vector<std::unique_ptr<Card>> cards;

    cards.push_back(CardFactory::create("Adept",  "Adept1",  1, 2, 3));
    cards.push_back(CardFactory::create("Spell",  "Spell1",  2, 0, 0, "HEAL", 5));
    cards.push_back(CardFactory::create("Legend", "Legend1", 5, 4, 6, "BUFF_BOTH", 2, "Titlu"));

    for (const auto& c : cards) {
        std::cout << "Tip: " << c->getType() << " | ";
        c->play();  
    }
}

static void demonstrateExceptions() {
    std::cout << "\n=== Exceptii proprii ===\n";
    try {
        throw InsufficientManaException(5, 2);
    } catch (const HearthstoneException& e) {
        std::cout << "  Prins: " << e.what() << "\n";
    }
    try {
        throw DeckFullException();
    } catch (const HearthstoneException& e) {
        std::cout << "  Prins: " << e.what() << "\n";
    }
    try {
        throw InvalidCardTypeException("Weapon");
    } catch (const HearthstoneException& e) {
        std::cout << "  Prins: " << e.what() << "\n";
    }
}

static void demonstrateSingleton() {
    std::cout << "\n=== Singleton GameLogger ===\n";
    GameLogger& log1 = GameLogger::getInstance();
    GameLogger& log2 = GameLogger::getInstance();
    std::cout << "  Acelasi logger: " << (&log1 == &log2 ? "DA" : "NU") << "\n";
    log1.onTurnStart("Alice", 1);
    log1.onMinionPlayed("Soldatul");
    log1.onPlayerDamaged("Bob", 3);
    std::cout << "  Evenimente logate in game_log.txt\n";
}

int main() {
    try {
        demonstrateRepositoryWithCards();
        demonstrateRepositoryWithPlayers();
        demonstratePolymorphism();
        demonstrateExceptions();
        demonstrateSingleton();

        std::cout << "\n=== Pornire Meniu ===\n";
        Menu menu;
        menu.run();

    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }
    return 0;
}