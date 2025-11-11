#pragma once

#include <string>
#include <utility>

class GameException : public std::exception {
protected:
    std::string msg;
public:
    explicit GameException(std::string  msg) : msg(std::move(msg)) {}
    [[nodiscard]] const char* what() const noexcept override { return msg.c_str(); }
};

class AssetLoadException : public GameException {
public:
    explicit AssetLoadException(const std::string& assetName) :
    GameException("EROARE: Nu am putut incarca resursa "+ assetName) {}
};

class MapLoadException : public GameException {
public:
    explicit MapLoadException(const std::string& mapFile, const std::string& details)
        : GameException("EROARE: Nu am putut incarca harta '" + mapFile + "'. Detalii: " + details) {}
};

class FontLoadException : public GameException {
public:
    explicit FontLoadException(const std::string& fontFile)
        : GameException("EROARE: Nu am putut incarca fontul " + fontFile) {}
};