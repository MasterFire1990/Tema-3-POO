#pragma once
#include "User.h"
#include "CardCollection.h"
#include "GameLogger.h"
#include <vector>
#include <memory>
#include <string>

class Menu {
    std::vector<std::unique_ptr<User>> users;
    CardCollection collection;
    std::shared_ptr<GameLogger> logger;

    static const std::string DATA_FOLDER;
    static const std::string USERS_FILE;

    void printSeparator() const;

    void menuUsers();
    void menuAddUser();
    void menuRemoveUser();
    void menuViewUsers();

    void menuCards();
    void menuAddCard();
    void menuRemoveCard();
    void menuViewCards();

    void menuDecks();
    void menuSelectUserForDeck(User*& selectedUser);
    void menuCreateDeck(User* user);
    void menuDeleteDeck(User* user);
    void menuBuildDeck(User* user);
    void menuViewDecks(User* user);

    void menuGame();
    void menuStats();

    void saveAllUsers() const;
    void loadAllUsers();

public:
    Menu();
    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;
    ~Menu() = default;

    void run();

    friend std::ostream& operator<<(std::ostream& os, const Menu& menu);
    friend std::istream& operator>>(std::istream& is, Menu& menu);
};