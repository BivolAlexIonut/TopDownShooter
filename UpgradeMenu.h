#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.h"

class UpgradeMenu {
public:
    UpgradeMenu();

    bool loadFont(const std::string& fontPath);
    void update(Player& player);
    void draw(sf::RenderWindow& window, sf::View uiView);

    void handleInput(sf::Keyboard::Key key, Player& player);
    std::string getFeedbackMessage();

private:
    void updateText(Player& player);

    sf::Font m_font;
    static sf::Font s_emptyMenuFont;
    sf::RectangleShape m_background;
    sf::Text m_titleText;
    sf::Text m_coinText;
    std::vector<sf::Text> m_menuItems;
    sf::Text m_feedbackText;
    sf::Clock m_feedbackClock;

    int m_selectedItemIndex;
};