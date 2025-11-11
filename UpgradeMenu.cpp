#include "UpgradeMenu.h"
#include <iostream>

sf::Font UpgradeMenu::s_emptyMenuFont;

UpgradeMenu::UpgradeMenu() : m_titleText(s_emptyMenuFont,""), m_coinText(s_emptyMenuFont,""),
m_menuItems(4, sf::Text(s_emptyMenuFont, "")), m_feedbackText(s_emptyMenuFont,""),m_selectedItemIndex(0) {
    m_background.setSize({1280.f, 720.f});
    m_background.setFillColor(sf::Color(0, 0, 0, 180));
}

bool UpgradeMenu::loadFont(const std::string& fontPath) {
    if (!m_font.openFromFile(fontPath)) {
        std::cerr << "EROARE: Nu am putut incarca fontul pentru meniu: " << fontPath << std::endl;
        return false;
    }
    return true;
}

void UpgradeMenu::updateText(const Player& player) {
    auto info = player.getUpgradeInfo();
    float viewWidth = 1280.f;
    float viewHeight = 720.f;

    m_titleText.setFont(m_font);
    m_titleText.setString("MENIU UPGRADE (Arma: " + info["weapon_name"] + ")");
    m_titleText.setCharacterSize(48);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setPosition({viewWidth / 2.f - m_titleText.getGlobalBounds().size.x/ 2.f, 50.f});

    m_coinText.setFont(m_font);
    m_coinText.setString("Bani: " + info["coin_count"]);
    m_coinText.setCharacterSize(36);
    m_coinText.setFillColor(sf::Color(240, 180, 0));
    m_coinText.setPosition({viewWidth / 2.f - m_coinText.getGlobalBounds().size.x / 2.f, 120.f});

    std::string labels[] = {
        "1. Damage (Cost: " + info["cost_damage"] + ") - Actual: " + info["damage"],
        "2. Viteza Tragere (Cost: " + info["cost_fire_rate"] + ") - Actual: " + info["fire_rate"],
        "3. Viteza Miscare (Cost: " + info["cost_move_speed"] + ") - Actual: " + info["move_speed"],
        "4. Viata Maxima (Cost: " + info["cost_health"] + ") - Actual: " + info["max_health"]
    };

    for (int i = 0; i < 4; ++i) {
        m_menuItems[i].setFont(m_font);
        m_menuItems[i].setString(labels[i]);
        m_menuItems[i].setCharacterSize(32);
        m_menuItems[i].setFillColor(sf::Color::White);
        m_menuItems[i].setPosition({viewWidth / 2.f - m_menuItems[i].getGlobalBounds().size.x / 2.f, 250.f +
            static_cast<float>(i) * 80.f});

        if (i == m_selectedItemIndex) {
            m_menuItems[i].setFillColor(sf::Color::Yellow);
        }
    }

    if (m_feedbackClock.getElapsedTime().asSeconds() > 2.0f) {
        m_feedbackText.setString("");
    }
    m_feedbackText.setFont(m_font);
    m_feedbackText.setCharacterSize(30);
    m_feedbackText.setFillColor(sf::Color::Red);
    m_feedbackText.setPosition({viewWidth / 2.f - m_feedbackText.getGlobalBounds().size.x / 2.f, viewHeight - 100.f});
}

void UpgradeMenu::update(Player& player) {
    updateText(player);
}

void UpgradeMenu::draw(sf::RenderWindow& window, sf::View uiView) {
    m_background.setPosition({uiView.getCenter().x - uiView.getSize().x / 2.f, uiView.getCenter().y - uiView.getSize().y / 2.f});

    window.draw(m_background);
    window.draw(m_titleText);
    window.draw(m_coinText);
    for (const auto& item : m_menuItems) {
        window.draw(item);
    }
    window.draw(m_feedbackText);
}

void UpgradeMenu::handleInput(sf::Keyboard::Key key, Player& player) {
    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W) {
        m_selectedItemIndex = (m_selectedItemIndex - 1 + 4) % 4;
    } else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S) {
        m_selectedItemIndex = (m_selectedItemIndex + 1) % 4;
    } else if (key == sf::Keyboard::Key::Enter) {
        std::string feedback;
        switch (m_selectedItemIndex) {
            case 0: feedback = player.upgradeCurrentWeaponDamage(); break;
            case 1: feedback = player.upgradeCurrentWeaponFireRate(); break;
            case 2: feedback = player.upgradeCurrentWeaponMoveSpeed(); break;
            case 3: feedback = player.upgradeMaxHealth(); break;
            default: ;
        }
        m_feedbackText.setString(feedback);
        m_feedbackClock.restart();
    }
}