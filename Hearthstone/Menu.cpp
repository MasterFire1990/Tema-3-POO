#include "../Hearthstone/Menu.h"
#include "../Hearthstone/GameEngine.h"
#include "../Hearthstone/Adept.h"
#include "../Hearthstone/Spell.h"
#include "../Hearthstone/Legend.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <limits>

static void cinClear() {
    if (std::cin.eof()) {
        std::cout << "\n  [INFO] EOF detected. Exiting...\n";
        std::exit(0);
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

const std::string Menu::DATA_FOLDER = "data";
const std::string Menu::USERS_FILE  = "data/users.txt";

void Menu::clearUsers() {
    for (User* u : users) delete u;
    users.clear();
}

void Menu::copyUsers(const std::vector<User*>& src) {
    for (User* u : src) users.push_back(new User(*u));
}

Menu::Menu() {
    std::filesystem::create_directories(DATA_FOLDER);
    try { collection.loadFromFiles(DATA_FOLDER); } catch (...) {}
    try { loadAllUsers(); } catch (...) {}
}

Menu::Menu(const Menu& other) : collection(other.collection) {
    copyUsers(other.users);
}

Menu& Menu::operator=(const Menu& other) {
    if (this != &other) {
        clearUsers();
        collection = other.collection;
        copyUsers(other.users);
    }
    return *this;
}

Menu::~Menu() { clearUsers(); }

void Menu::printSeparator() const {
    std::cout << "  --------------------------------------------------\n";
}



void Menu::menuAddUser() {
    std::cout << "\n  Nume utilizator nou: ";
    std::string name;
    std::getline(std::cin >> std::ws, name);
    for (User* u : users)
        if (u->getUsername() == name) {
            std::cout << "  [ERR] Utilizatorul exista deja.\n";
            return;
        }
    users.push_back(new User(name));
    saveAllUsers();
    std::cout << "  [OK] Utilizator '" << name << "' adaugat.\n";
}

void Menu::menuRemoveUser() {
    if (users.empty()) { std::cout << "  [INFO] Nu exista utilizatori.\n"; return; }
    menuViewUsers();
    std::cout << "  Index utilizator de sters (0 = renunta): ";
    int idx;
    if (!(std::cin >> idx)) { cinClear(); std::cout << "  [ERR] Input invalid.\n"; return; }
    cinClear();
    if (idx == 0) return;
    idx--;
    if (idx < 0 || idx >= static_cast<int>(users.size())) {
        std::cout << "  [ERR] Index invalid.\n"; return;
    }
    std::string name = users[idx]->getUsername();
    delete users[idx];
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
        std::cout << "\n  === UTILIZATORI ===\n";
        std::cout << "  [1] Adauga utilizator\n";
        std::cout << "  [2] Sterge utilizator\n";
        std::cout << "  [3] Vizualizeaza utilizatori\n";
        std::cout << "  [0] Inapoi\n  > ";
        if (!(std::cin >> opt)) {
            cinClear();
            continue;
        }
        cinClear();
        try {
            if (opt == 1) menuAddUser();
            else if (opt == 2) menuRemoveUser();
            else if (opt == 3) menuViewUsers();
        } catch (const std::exception& e) {
            std::cout << "  [ERR] " << e.what() << "\n";
        }
    } while (opt != 0);
}



void Menu::menuAddCard() {
    std::cout << "\n  Tip carte (1=Adept, 2=Spell, 3=Legend): ";
    int type;
    if (!(std::cin >> type)) { cinClear(); std::cout << "  [ERR] Input invalid.\n"; return; }
    cinClear();
    Card* card = nullptr;
    try {
        if (type == 1) {
            Adept* a = new Adept();
            std::cin >> *a;
            card = a;
        } else if (type == 2) {
            Spell* s = new Spell();
            std::cin >> *s;
            card = s;
        } else if (type == 3) {
            Legend* ch = new Legend();
            std::cin >> *ch;
            card = ch;
        } else {
            std::cout << "  [ERR] Tip invalid.\n"; return;
        }
        collection.addCard(card);
        delete card;
        collection.saveToFiles(DATA_FOLDER);
        std::cout << "  [OK] Carte adaugata.\n";
    } catch (const std::exception& e) {
        delete card;
        card = nullptr;
        std::cin.clear();
        std::cout << "  [ERR] " << e.what() << "\n";
    }
}

void Menu::menuRemoveCard() {
    if (collection.getCount() == 0) { std::cout << "  [INFO] Nu exista carti.\n"; return; }
    std::cout << collection;
    std::cout << "  Index carte de sters (0 = renunta): ";
    int idx;
    if (!(std::cin >> idx)) { cinClear(); std::cout << "  [ERR] Input invalid.\n"; return; }
    cinClear();
    if (idx == 0) return;
    try {
        collection.removeCard(idx - 1);
        collection.saveToFiles(DATA_FOLDER);
        std::cout << "  [OK] Carte stearsa.\n";
    } catch (const std::exception& e) {
        std::cout << "  [ERR] " << e.what() << "\n";
    }
}

void Menu::menuViewCards() {
    if (collection.getCount() == 0) { std::cout << "  [INFO] Nu exista carti in colectie.\n"; return; }
    std::cout << collection;
}

void Menu::menuCards() {
    int opt;
    do {
        std::cout << "\n  === CARTI ===\n";
        std::cout << "  [1] Adauga carte\n";
        std::cout << "  [2] Sterge carte\n";
        std::cout << "  [3] Vizualizeaza toate cartile\n";
        std::cout << "  [0] Inapoi\n  > ";
        if (!(std::cin >> opt)) {
            cinClear();
            opt = -1;
            continue;
        }
        cinClear();
        try {
            if (opt == 1) menuAddCard();
            else if (opt == 2) menuRemoveCard();
            else if (opt == 3) menuViewCards();
        } catch (const std::exception& e) {
            std::cout << "  [ERR] " << e.what() << "\n";
        }
    } while (opt != 0);
}



void Menu::menuSelectUserForDeck(User*& selectedUser) {
    if (users.empty()) { std::cout << "  [INFO] Nu exista utilizatori.\n"; return; }
    menuViewUsers();
    std::cout << "  Selecteaza utilizator: ";
    int idx; std::cin >> idx;
    if (idx < 1 || idx > static_cast<int>(users.size())) {
        std::cout << "  [ERR] Index invalid.\n"; return;
    }
    selectedUser = users[idx - 1];
}

void Menu::menuCreateDeck(User* user) {
    std::cout << "  Nume deck nou: ";
    std::string name; std::getline(std::cin >> std::ws, name);
    try {
        user->addDeck(name);
        saveAllUsers();
        std::cout << "  [OK] Deck '" << name << "' creat.\n";
    } catch (const std::exception& e) {
        std::cout << "  [ERR] " << e.what() << "\n";
    }
}

void Menu::menuDeleteDeck(User* user) {
    if (user->getDeckCount() == 0) { std::cout << "  [INFO] Utilizatorul nu are deck-uri.\n"; return; }
    menuViewDecks(user);
    std::cout << "  Index deck de sters (0 = renunta): ";
    int idx; std::cin >> idx;
    if (idx == 0) return;
    try {
        user->removeDeck(idx - 1);
        saveAllUsers();
        std::cout << "  [OK] Deck sters.\n";
    } catch (const std::exception& e) {
        std::cout << "  [ERR] " << e.what() << "\n";
    }
}

void Menu::menuBuildDeck(User* user) {
    if (user->getDeckCount() == 0) { std::cout << "  [INFO] Creeaza mai intai un deck.\n"; return; }
    menuViewDecks(user);
    std::cout << "  Selecteaza deck: ";
    int didx;
    if (!(std::cin >> didx)) { cinClear(); std::cout << "  [ERR] Input invalid.\n"; return; }
    cinClear();
    if (didx < 1 || didx > user->getDeckCount()) { std::cout << "  [ERR] Index invalid.\n"; return; }
    Deck* deck = user->getDeck(didx - 1);

    int opt;
    do {
        std::cout << "\n  Deck: " << deck->getDeckName() << " (" << deck->getSize() << "/30)\n";
        std::cout << "  [1] Adauga carte din colectie\n";
        std::cout << "  [2] Sterge carte din deck\n";
        std::cout << "  [3] Vizualizeaza deck\n";
        std::cout << "  [0] Salveaza si iesi\n  > ";
        if (!(std::cin >> opt)) {
            cinClear();
            opt = -1;
            continue;
        }
        cinClear();
        try {
            if (opt == 1) {
                if (collection.getCount() == 0) { std::cout << "  [INFO] Colectia e goala.\n"; continue; }
                std::cout << collection;
                std::cout << "  Index carte de adaugat: ";
                int cidx;
                if (!(std::cin >> cidx)) { cinClear(); std::cout << "  [ERR] Input invalid.\n"; continue; }
                cinClear();
                deck->addCard(collection.getCard(cidx - 1));
                saveAllUsers();
                std::cout << "  [OK] Carte adaugata in deck.\n";
            } else if (opt == 2) {
                if (deck->getSize() == 0) { std::cout << "  [INFO] Deck-ul e gol.\n"; continue; }
                std::cout << *deck;
                std::cout << "  Index carte de scos: ";
                int ridx;
                if (!(std::cin >> ridx)) { cinClear(); std::cout << "  [ERR] Input invalid.\n"; continue; }
                cinClear();
                deck->removeCard(ridx - 1);
                saveAllUsers();
                std::cout << "  [OK] Carte scoasa.\n";
            } else if (opt == 3) {
                std::cout << *deck;
            }
        } catch (const std::exception& e) {
            std::cout << "  [ERR] " << e.what() << "\n";
        }
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
        std::cout << "\n  === DECK-URI ===\n";
        std::cout << "  [1] Creeaza deck\n";
        std::cout << "  [2] Sterge deck\n";
        std::cout << "  [3] Editeaza deck\n";
        std::cout << "  [4] Vizualizeaza deck-uri utilizator\n";
        std::cout << "  [0] Inapoi\n  > ";
        if (!(std::cin >> opt)) {
            cinClear();
            opt = -1;
            continue;
        }
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
        } catch (const std::exception& e) {
            std::cout << "  [ERR] " << e.what() << "\n";
        }
    } while (opt != 0);
}



void Menu::menuGame() {
    if (users.size() < 2) {
        std::cout << "  [INFO] Ai nevoie de cel putin 2 utilizatori.\n"; return;
    }
    std::cout << "\n  === START JOC ===\n";
    std::cout << "  Selecteaza Player 1:\n";
    menuViewUsers();
    std::cout << "  > "; int u1; std::cin >> u1; u1--;
    if (u1 < 0 || u1 >= static_cast<int>(users.size())) { std::cout << "  [ERR] Invalid.\n"; return; }

    std::cout << "  Selecteaza Player 2:\n";
    menuViewUsers();
    std::cout << "  > "; int u2; std::cin >> u2; u2--;
    if (u2 < 0 || u2 >= static_cast<int>(users.size()) || u1 == u2) {
        std::cout << "  [ERR] Jucatori invalizi sau identici.\n"; return;
    }

    User* p1 = users[u1];
    User* p2 = users[u2];
    if (p1->getDeckCount() == 0 || p2->getDeckCount() == 0) {
        std::cout << "  [ERR] Ambii jucatori trebuie sa aiba cel putin un deck.\n"; return;
    }

    std::cout << "\n  Deck-uri " << p1->getUsername() << ":\n";
    menuViewDecks(p1);
    std::cout << "  Selecteaza deck: "; int d1; std::cin >> d1; d1--;
    if (d1 < 0 || d1 >= p1->getDeckCount()) { std::cout << "  [ERR] Invalid.\n"; return; }

    std::cout << "\n  Deck-uri " << p2->getUsername() << ":\n";
    menuViewDecks(p2);
    std::cout << "  Selecteaza deck: "; int d2; std::cin >> d2; d2--;
    if (d2 < 0 || d2 >= p2->getDeckCount()) { std::cout << "  [ERR] Invalid.\n"; return; }

    try {
        GameEngine engine(p1, p1->getDeck(d1), p2, p2->getDeck(d2));
        engine.startGame();
    } catch (const std::exception& e) {
        std::cout << "  [ERR] " << e.what() << "\n";
    }
}



void Menu::saveAllUsers() const {
    std::filesystem::create_directories(DATA_FOLDER);
    std::ofstream f(USERS_FILE);
    if (!f) throw std::runtime_error("Cannot save users list");
    f << users.size() << "\n";
    for (const User* u : users) {
        f << u->getUsername() << "\n";
        try { u->saveToFile(DATA_FOLDER); } catch (...) {}
    }
}

void Menu::loadAllUsers() {
    std::ifstream f(USERS_FILE);
    if (!f) return;
    clearUsers();
    int count; f >> count; f.ignore();
    for (int i = 0; i < count; i++) {
        std::string name; std::getline(f, name);
        User* u = new User(name);
        try { u->loadFromFile(DATA_FOLDER); } catch (...) {}
        users.push_back(u);
    }
}



void Menu::run() {
    int opt;
    do {
        std::cout << "\n=== HEARTHSTONE ===\n";
        printSeparator();
        std::cout << "  [1] Utilizatori\n";
        std::cout << "  [2] Carti\n";
        std::cout << "  [3] Deck-uri\n";
        std::cout << "  [4] Start Joc\n";
        std::cout << "  [0] Iesire\n";
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
        } catch (const std::exception& e) {
            std::cout << "  [ERR] " << e.what() << "\n";
        }
    } while (opt != 0);
    std::cout << "  La revedere!\n";
}

std::ostream& operator<<(std::ostream& os, const Menu& menu) {
    os << "Menu | Users: " << menu.users.size()
       << " | Cards in collection: " << menu.collection.getCount() << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Menu& menu) {
    (void)menu;
    return is;
}