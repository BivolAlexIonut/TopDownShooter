#include "Game.h"
#include <iostream>

int main() {
    try {
        Game::getInstance().run();
    } catch (const std::exception& e) {
        std::cerr << "EROARE FATALA: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}