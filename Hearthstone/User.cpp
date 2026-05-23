#include "../Hearthstone/User.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <filesystem>

void User::clearDecks() {
    for (Deck* d : decks) delete d;
    decks.clear();
}

void User::copyDecks(const std::vector<Deck*>& src) {
    for (Deck* d : src)
        decks.push_back(new Deck(*d));
}

User::User() : username("Unknown") {}

User::User(const std::string& name) {
    setUsername(name);
}

User::User(const User& other) : username(other.username) {
    copyDecks(other.decks);
}

User& User::operator=(const User& other) {
    if (this != &other) {
        clearDecks();
        username = other.username;
        copyDecks(other.decks);
    }
    return *this;
}

User::~User() { clearDecks(); }

std::string User::getUsername() const { return username; }

void User::setUsername(const std::string& name) {
    if (name.empty()) throw std::invalid_argument("Username cannot be empty");
    username = name;
}

int User::getDeckCount() const { return static_cast<int>(decks.size()); }

Deck* User::getDeck(int index) const {
    if (index < 0 || index >= static_cast<int>(decks.size()))
        throw std::out_of_range("Invalid deck index");
    return decks[index];
}

Deck* User::getDeckByName(const std::string& name) const {
    for (Deck* d : decks)
        if (d->getDeckName() == name) return d;
    return nullptr;
}

void User::addDeck(const std::string& deckName) {
    if (getDeckByName(deckName) != nullptr)
        throw std::runtime_error("Deck with this name already exists");
    decks.push_back(new Deck(deckName, username));
}

void User::removeDeck(int index) {
    if (index < 0 || index >= static_cast<int>(decks.size()))
        throw std::out_of_range("Invalid deck index");
    delete decks[index];
    decks.erase(decks.begin() + index);
}

void User::saveToFile(const std::string& folder) const {
    std::filesystem::create_directories(folder);
    std::ofstream meta(folder + "/" + username + "_meta.txt");
    if (!meta) throw std::runtime_error("Cannot save user meta");
    meta << username << "\n" << decks.size() << "\n";
    for (const Deck* d : decks) {
        meta << d->getDeckName() << "\n";
        d->saveToFile(folder + "/" + username + "_" + d->getDeckName() + ".txt");
    }
}

void User::loadFromFile(const std::string& folder) {
    std::ifstream meta(folder + "/" + username + "_meta.txt");
    if (!meta) throw std::runtime_error("No save file for user: " + username);
    clearDecks();
    std::string uname;
    int count;
    std::getline(meta, uname);
    meta >> count;
    meta.ignore();
    for (int i = 0; i < count; i++) {
        std::string dname;
        std::getline(meta, dname);
        Deck* d = new Deck(dname, username);
        try {
            d->loadFromFile(folder + "/" + username + "_" + dname + ".txt");
        } catch (...) {}
        decks.push_back(d);
    }
}

std::ostream& operator<<(std::ostream& os, const User& user) {
    os << "User: " << user.username << " | Decks: " << user.decks.size() << "\n";
    for (int i = 0; i < static_cast<int>(user.decks.size()); i++)
        os << "  " << (i + 1) << ". " << user.decks[i]->getDeckName()
           << " (" << user.decks[i]->getSize() << "/30)\n";
    return os;
}

std::istream& operator>>(std::istream& is, User& user) {
    std::cout << "Username: ";
    std::string name;
    std::getline(is >> std::ws, name);
    user.setUsername(name);
    return is;
}