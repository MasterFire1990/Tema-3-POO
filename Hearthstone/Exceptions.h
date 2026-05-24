#pragma once
#include <stdexcept>
#include <string>

class HearthstoneException : public std::exception {
    std::string message;
public:
    explicit HearthstoneException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class InsufficientManaException : public HearthstoneException {
public:
    InsufficientManaException(int cost, int available)
        : HearthstoneException("Mana insuficienta! Cost: " +
            std::to_string(cost) + ", ai: " + std::to_string(available)) {}
};

class BoardFullException : public HearthstoneException {
public:
    BoardFullException()
        : HearthstoneException("Board plin! Nu poti pune mai mult de 7 minioni.") {}
};

class DeckFullException : public HearthstoneException {
public:
    DeckFullException()
        : HearthstoneException("Deck plin! Maxim 30 de carti.") {}
};

class InvalidIndexException : public HearthstoneException {
public:
    explicit InvalidIndexException(const std::string& context)
        : HearthstoneException("Index invalid: " + context) {}
};

class DuplicateCardException : public HearthstoneException {
public:
    explicit DuplicateCardException(const std::string& name)
        : HearthstoneException("Carta exista deja in colectie: " + name) {}
};

class InvalidCardTypeException : public HearthstoneException {
public:
    explicit InvalidCardTypeException(const std::string& type)
        : HearthstoneException("Tip de carta necunoscut: " + type) {}
};