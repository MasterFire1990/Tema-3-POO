#include "../Hearthstone/Menu.h"
#include "../Hearthstone/GameEngine.h"
#include "../Hearthstone/CardFactory.h"
#include "../Hearthstone/Exceptions.h"
#include "../Hearthstone/Repository.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <limits>

const std::string Menu::DATA_FOLDER = "data";
const std::string Menu::USERS_FILE  = "data/users.txt";

static void cinClear() {
    if (std::cin.eof()) { std::cout << "\n  [INFO] EOF. Iesire...\n"; std::exit(0); }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

Menu::Menu() {
    std::filesystem::create_directories(DATA_FOLDER);
    // Singleton logger — shared_ptr wrapping the singleton reference
    logger = std::shared_ptr<GameLogger>(&GameLogger::getInstance(), [](GameLogger*){});
    try { collection.loadFromFiles(DATA_FOLDER); } catch (...) {}
    try { loadAllUsers(); } catch (...) {}
}

void Menu::printSeparator() const {
    std::cout << "  --------------------------------------------------\n";
}

// ─── USERS ────────────────────────────────────────────────────────────────────

void Menu::menuAddUser() {
    std::cout << "\n  Nume utilizator nou: ";
    std::string name;
    std::getline(std::cin >> std::ws, name);
    for (const auto& u : users)
        if (u->getName() == name) {
            std::cout << "  [ERR] Utilizatorul exista deja.\n"; return;
        }
    users.push_back(std::make_unique<User>(name));
    saveAllUsers();
    std::cout << "  [OK] Utilizator '" << name << "' adaugat.\n";
}

void Menu::menuRemoveUser() {
    if (users.empty()) { std::cout << "  [INFO] Nu exista utilizatori.\n"; return; }
    menuViewUsers();
    std::cout << "  Index utilizator de sters (0 = renunta): ";
    int idx;
    if (!(std::cin >> idx)) { cinClear(); return; }
    cinClear();
    if (idx == 0) return;
    idx--;
    if (idx < 0 || idx >= static_cast<int>(users.size())) {
        std::cout << "  [ERR] Index invalid.\n"; return;
    }
    std::string name = users[idx]->getName();
    users.erase(users.begin() + idx);
    saveAllUsers();
    std::cout << "  [OK] Utilizator '" << name << "' sters.\n";
}

void Menu::menuViewUsers() {
    if (users.empty()) { std::cout << "  [INFO] Nu exista utilizatori.\n"; return; }
    printSeparator();
    for (int i = 0; i < static_cast<int>(users.size()); i++)
        std::cout << "  " << (i + 1) << ". " << *users[i];
    printSeparator();
}

void Menu::menuUsers() {
    int opt;
    do {
        std::cout << "\n  === UTILIZATORI ===\n"
                  << "  [1] Adauga  [2] Sterge  [3] Vizualizeaza  [0] Inapoi\n  > ";
        if (!(std::cin >> opt)) { cinClear(); continue; }
        cinClear();
        try {
            if (opt == 1) menuAddUser();
            else if (opt == 2) menuRemoveUser();
            else if (opt == 3) menuViewUsers();
        } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
    } while (opt != 0);
}

// ─── CARDS ────────────────────────────────────────────────────────────────────

void Menu::menuAddCard() {
    std::cout << "\n  Tip carte (1=Adept, 2=Spell, 3=Legend): ";
    int type;
    if (!(std::cin >> type)) { cinClear(); return; }
    cinClear();
    std::string typeName;
    if (type == 1) typeName = "Adept";
    else if (type == 2) typeName = "Spell";
    else if (type == 3) typeName = "Legend";
    else { std::cout << "  [ERR] Tip invalid.\n"; return; }

    try {
        auto card = CardFactory::createEmpty(typeName);
        std::cin >> *card;
        collection.addCard(*card);
        collection.saveToFiles(DATA_FOLDER);
        std::cout << "  [OK] Carte adaugata.\n";
    } catch (const HearthstoneException& e) {
        std::cin.clear();
        std::cout << "  [ERR] " << e.what() << "\n";
    }
}

void Menu::menuRemoveCard() {
    if (collection.getCount() == 0) { std::cout << "  [INFO] Nu exista carti.\n"; return; }
    std::cout << collection;
    std::cout << "  Index carte de sters (0 = renunta): ";
    int idx;
    if (!(std::cin >> idx)) { cinClear(); return; }
    cinClear();
    if (idx == 0) return;
    try {
        collection.removeCard(idx - 1);
        collection.saveToFiles(DATA_FOLDER);
        std::cout << "  [OK] Carte stearsa.\n";
    } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
}

void Menu::menuViewCards() {
    if (collection.getCount() == 0) { std::cout << "  [INFO] Colectia e goala.\n"; return; }
    std::cout << collection;
}

void Menu::menuCards() {
    int opt;
    do {
        std::cout << "\n  === CARTI ===\n"
                  << "  [1] Adauga  [2] Sterge  [3] Vizualizeaza  [0] Inapoi\n  > ";
        if (!(std::cin >> opt)) { cinClear(); opt = -1; continue; }
        cinClear();
        try {
            if (opt == 1) menuAddCard();
            else if (opt == 2) menuRemoveCard();
            else if (opt == 3) menuViewCards();
        } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
    } while (opt != 0);
}

// ─── DECKS ────────────────────────────────────────────────────────────────────

void Menu::menuSelectUserForDeck(User*& selectedUser) {
    if (users.empty()) { std::cout << "  [INFO] Nu exista utilizatori.\n"; return; }
    menuViewUsers();
    std::cout << "  Selecteaza utilizator: ";
    int idx; std::cin >> idx; cinClear();
    if (idx < 1 || idx > static_cast<int>(users.size())) {
        std::cout << "  [ERR] Index invalid.\n"; return;
    }
    selectedUser = users[idx - 1].get();
}

void Menu::menuCreateDeck(User* user) {
    std::cout << "  Nume deck nou: ";
    std::string name; std::getline(std::cin >> std::ws, name);
    try {
        user->addDeck(name);
        saveAllUsers();
        std::cout << "  [OK] Deck '" << name << "' creat.\n";
    } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
}

void Menu::menuDeleteDeck(User* user) {
    if (user->getDeckCount() == 0) { std::cout << "  [INFO] Nu are deck-uri.\n"; return; }
    menuViewDecks(user);
    std::cout << "  Index deck de sters (0 = renunta): ";
    int idx; std::cin >> idx; cinClear();
    if (idx == 0) return;
    try {
        user->removeDeck(idx - 1);
        saveAllUsers();
        std::cout << "  [OK] Deck sters.\n";
    } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
}

void Menu::menuBuildDeck(User* user) {
    if (user->getDeckCount() == 0) { std::cout << "  [INFO] Creeaza mai intai un deck.\n"; return; }
    menuViewDecks(user);
    std::cout << "  Selecteaza deck: ";
    int didx; std::cin >> didx; cinClear();
    if (didx < 1 || didx > user->getDeckCount()) { std::cout << "  [ERR] Index invalid.\n"; return; }
    Deck* deck = user->getDeck(didx - 1);

    int opt;
    do {
        std::cout << "\n  Deck: " << deck->getDeckName() << " (" << deck->getSize() << "/30)\n"
                  << "  [1] Adauga carte  [2] Sterge carte  [3] Vizualizeaza  [0] Iesi\n  > ";
        if (!(std::cin >> opt)) { cinClear(); opt = -1; continue; }
        cinClear();
        try {
            if (opt == 1) {
                if (collection.getCount() == 0) { std::cout << "  [INFO] Colectia e goala.\n"; continue; }
                std::cout << collection;
                std::cout << "  Index carte: ";
                int cidx; std::cin >> cidx; cinClear();
                deck->addCard(*collection.getCard(cidx - 1));
                saveAllUsers();
                std::cout << "  [OK] Carte adaugata.\n";
            } else if (opt == 2) {
                if (deck->getSize() == 0) { std::cout << "  [INFO] Deck gol.\n"; continue; }
                std::cout << *deck;
                std::cout << "  Index carte de scos: ";
                int ridx; std::cin >> ridx; cinClear();
                deck->removeCard(ridx - 1);
                saveAllUsers();
                std::cout << "  [OK] Carte scoasa.\n";
            } else if (opt == 3) {
                std::cout << *deck;
            }
        } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
    } while (opt != 0);
}

void Menu::menuViewDecks(User* user) {
    if (user->getDeckCount() == 0) { std::cout << "  [INFO] Niciun deck.\n"; return; }
    for (int i = 0; i < user->getDeckCount(); i++)
        std::cout << "  " << (i + 1) << ". " << *user->getDeck(i);
}

void Menu::menuDecks() {
    int opt;
    do {
        std::cout << "\n  === DECK-URI ===\n"
                  << "  [1] Creeaza  [2] Sterge  [3] Editeaza  [4] Vizualizeaza  [0] Inapoi\n  > ";
        if (!(std::cin >> opt)) { cinClear(); opt = -1; continue; }
        cinClear();
        try {
            if (opt >= 1 && opt <= 4) {
                User* user = nullptr;
                menuSelectUserForDeck(user);
                if (!user) continue;
                if (opt == 1) menuCreateDeck(user);
                else if (opt == 2) menuDeleteDeck(user);
                else if (opt == 3) menuBuildDeck(user);
                else if (opt == 4) menuViewDecks(user);
            }
        } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
    } while (opt != 0);
}

// ─── STATS ────────────────────────────────────────────────────────────────────

void Menu::menuStats() {
    std::cout << "\n  === STATISTICI ===\n";
    std::cout << "  Utilizatori: " << users.size() << "\n";
    std::cout << "  Carti in colectie: " << collection.getCount() << "\n";

    // Template function countIf used here — counts cards by type
    int adepts  = countIf<Card>(collection.getRepository(),
        [](const Card& c) { return c.getType() == "Adept"; });
    int spells  = countIf<Card>(collection.getRepository(),
        [](const Card& c) { return c.getType() == "Spell"; });
    int legends = countIf<Card>(collection.getRepository(),
        [](const Card& c) { return c.getType() == "Legend"; });

    std::cout << "    Adepti: " << adepts
              << "  Spelluri: " << spells
              << "  Legende: " << legends << "\n";

    int totalDecks = 0;
    for (const auto& u : users) totalDecks += u->getDeckCount();
    std::cout << "  Total deck-uri: " << totalDecks << "\n";
}

// ─── GAME ─────────────────────────────────────────────────────────────────────

void Menu::menuGame() {
    if (users.size() < 2) { std::cout << "  [INFO] Ai nevoie de cel putin 2 utilizatori.\n"; return; }
    std::cout << "\n  === START JOC ===\n  Selecteaza Player 1:\n";
    menuViewUsers();
    std::cout << "  > "; int u1; std::cin >> u1; u1--;
    if (u1 < 0 || u1 >= static_cast<int>(users.size())) { std::cout << "  [ERR] Invalid.\n"; return; }

    std::cout << "  Selecteaza Player 2:\n";
    menuViewUsers();
    std::cout << "  > "; int u2; std::cin >> u2; u2--;
    if (u2 < 0 || u2 >= static_cast<int>(users.size()) || u1 == u2) {
        std::cout << "  [ERR] Jucatori invalizi sau identici.\n"; return;
    }

    User* p1 = users[u1].get();
    User* p2 = users[u2].get();
    if (p1->getDeckCount() == 0 || p2->getDeckCount() == 0) {
        std::cout << "  [ERR] Ambii jucatori trebuie sa aiba cel putin un deck.\n"; return;
    }

    std::cout << "\n  Deck-uri " << p1->getName() << ":\n"; menuViewDecks(p1);
    std::cout << "  Selecteaza deck: "; int d1; std::cin >> d1; d1--;
    if (d1 < 0 || d1 >= p1->getDeckCount()) { std::cout << "  [ERR] Invalid.\n"; return; }

    std::cout << "\n  Deck-uri " << p2->getName() << ":\n"; menuViewDecks(p2);
    std::cout << "  Selecteaza deck: "; int d2; std::cin >> d2; d2--;
    if (d2 < 0 || d2 >= p2->getDeckCount()) { std::cout << "  [ERR] Invalid.\n"; return; }

    try {
        GameEngine engine(p1, p1->getDeck(d1), p2, p2->getDeck(d2));
        engine.addObserver(logger);
        engine.startGame();
    } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
}

// ─── PERSISTENCE ──────────────────────────────────────────────────────────────

void Menu::saveAllUsers() const {
    std::filesystem::create_directories(DATA_FOLDER);
    std::ofstream f(USERS_FILE);
    if (!f) throw HearthstoneException("Cannot save users list");
    f << users.size() << "\n";
    for (const auto& u : users) {
        f << u->getName() << "\n";
        try { u->saveToFile(DATA_FOLDER); } catch (...) {}
    }
}

void Menu::loadAllUsers() {
    std::ifstream f(USERS_FILE);
    if (!f) return;
    users.clear();
    int count; f >> count; f.ignore();
    for (int i = 0; i < count; i++) {
        std::string name; std::getline(f, name);
        auto u = std::make_unique<User>(name);
        try { u->loadFromFile(DATA_FOLDER); } catch (...) {}
        users.push_back(std::move(u));
    }
}

// ─── MAIN LOOP ────────────────────────────────────────────────────────────────

void Menu::run() {
    int opt;
    do {
        std::cout << "\n=== HEARTHSTONE ===\n";
        printSeparator();
        std::cout << "  [1] Utilizatori\n  [2] Carti\n  [3] Deck-uri\n"
                  << "  [4] Start Joc\n  [5] Statistici\n  [0] Iesire\n";
        printSeparator();
        std::cout << "  > ";
        if (!(std::cin >> opt)) {
            if (std::cin.eof()) break;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        try {
            if (opt == 1) menuUsers();
            else if (opt == 2) menuCards();
            else if (opt == 3) menuDecks();
            else if (opt == 4) menuGame();
            else if (opt == 5) menuStats();
        } catch (const HearthstoneException& e) { std::cout << "  [ERR] " << e.what() << "\n"; }
    } while (opt != 0);
    std::cout << "  La revedere!\n";
}

std::ostream& operator<<(std::ostream& os, const Menu& menu) {
    os << "Menu | Users: " << menu.users.size()
       << " | Cards: " << menu.collection.getCount() << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Menu& menu) { (void)menu; return is; }