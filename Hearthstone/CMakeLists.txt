cmake_minimum_required(VERSION 3.20)
project(Hearthstone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        main.cpp
        Card.cpp
        Adept.cpp
        Spell.cpp
        Legend.cpp
        Player.cpp
        Deck.cpp
        User.cpp
        CardCollection.cpp
        Board.cpp
        GameEngine.cpp
        GameLogger.cpp
        CardFactory.cpp
        Menu.cpp
)

add_executable(hearthstone ${SOURCES})

target_include_directories(hearthstone PRIVATE ${CMAKE_SOURCE_DIR})

if(MSVC)
    target_compile_options(hearthstone PRIVATE /W4 /std:c++20)
else()
    target_compile_options(hearthstone PRIVATE -Wall -Wextra -Wpedantic)
endif()