cmake_minimum_required(VERSION 4.1)
project(Hearthstone)

set(CMAKE_CXX_STANDARD 20)

add_executable(Hearthstone main.cpp
        Card.cpp
        Card.h
        Adept.cpp
        Adept.h
        Spell.cpp
        Spell.h
        Legend.cpp
        Legend.h
        User.cpp
        User.h
        CardCollection.cpp
        CardCollection.h
        Player.cpp
        Player.h
        Board.cpp
        Board.h
        GameEngine.cpp
        GameEngine.h
        Menu.cpp
        Menu.h
        Card.cpp
        Card.h
        Deck.cpp
        Deck.h)
