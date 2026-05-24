#include "../Hearthstone/User.h"
#include "../Hearthstone/Exceptions.h"
#include <fstream>
#include <iostream>
#include <filesystem>

User::User() : username("Unknown") {}

User::User(const std::string& name) {
    setUsername(name);
}

User::User(const User& other) : username(other.username) {
    for (const auto& d : other.decks)
        decks.push_back(std::make_unique<Deck>(*d));
}

User& User::operator=(const User& other) {
    if (this != &other) {
        username = other.username;
        decks.clear();
        for (const auto& d : other.decks)
            decks.push_back(std::make_unique<Deck>(*d));
    }
    return *this;
}

std::string User::getName() const { return username; }

void User::setUsername(const std::string& name) {
    if (name.empty()) throw HearthstoneException("Username cannot be empty");
    username = name;
}

int User::getDeckCount() const { return static_cast<int>(decks.size()); }

Deck* User::getDeck(int index) const {
    if (index < 0 || index >= static_cast<int>(decks.size()))
        throw InvalidIndexException("User::getDeck");
    return decks[index].get();
}

Deck* User::getDeckByName(const std::string& name) const {
    for (const auto& d : decks)
        if (d->getDeckName() == name) return d.get();
    return nullptr;
}

void User::addDeck(const std::string& deckName) {
    if (getDeckByName(deckName))
        throw HearthstoneException("Deck cu acest nume exista deja");
    decks.push_back(std::make_unique<Deck>(deckName, username));
}

void User::removeDeck(int index) {
    if (index < 0 || index >= static_cast<int>(decks.size()))
        throw InvalidIndexException("User::removeDeck");
    decks.erase(decks.begin() + index);
}

void User::saveToFile(const std::string& folder) const {
    std::filesystem::create_directories(folder);
    std::ofstream meta(folder + "/" + username + "_meta.txt");
    if (!meta) throw HearthstoneException("Cannot save user meta");
    meta << username << "\n" << decks.size() << "\n";
    for (const auto& d : decks) {
        meta << d->getDeckName() << "\n";
        d->saveToFile(folder + "/" + username + "_" + d->getDeckName() + ".txt");
    }
}

void User::loadFromFile(const std::string& folder) {
    std::ifstream meta(folder + "/" + username + "_meta.txt");
    if (!meta) throw HearthstoneException("No save file for user: " + username);
    decks.clear();
    std::string uname; int count;
    std::getline(meta, uname);
    meta >> count; meta.ignore();
    for (int i = 0; i < count; i++) {
        std::string dname; std::getline(meta, dname);
        auto d = std::make_unique<Deck>(dname, username);
        try { d->loadFromFile(folder + "/" + username + "_" + dname + ".txt"); }
        catch (...) {}
        decks.push_back(std::move(d));
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