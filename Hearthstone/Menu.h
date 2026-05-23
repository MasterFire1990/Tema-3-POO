#pragma once
#include "User.h"
#include "CardCollection.h"
#include <vector>
#include <string>

class Menu {
    std::vector<User*> users;
    CardCollection collection;

    static const std::string DATA_FOLDER;
    static const std::string USERS_FILE;

    void clearUsers();
    void copyUsers(const std::vector<User*>& src);

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

    void saveAllUsers() const;
    void loadAllUsers();

public:
    Menu();
    Menu(const Menu& other);
    Menu& operator=(const Menu& other);
    ~Menu();

    void run();

    friend std::ostream& operator<<(std::ostream& os, const Menu& menu);
    friend std::istream& operator>>(std::istream& is, Menu& menu);
};