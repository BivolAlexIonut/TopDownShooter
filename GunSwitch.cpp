#include "GunSwitch.h"
#include <iostream>

GunSwitch::GunSwitch() : m_currentWeaponIndex(0) {
    m_weaponRects.push_back(sf::IntRect({1869, 1425}, {262, 356}));
    m_weaponRects.push_back(sf::IntRect({1467, 1364}, {266, 468}));
    m_weaponRects.push_back(sf::IntRect({267, 1326}, {266, 548}));
    m_weaponRects.push_back(sf::IntRect({667, 1404}, {266, 398}));
    m_weaponRects.push_back(sf::IntRect({1067, 1356}, {266, 491}));
    m_weaponRects.push_back(sf::IntRect({2267, 1324}, {266, 558}));

    std::cout << "DEBUG: GunSwitch created with " << m_weaponRects.size() << " weapons." << std::endl;
}

int GunSwitch::getCurrentWeaponIndex() const {
    return m_currentWeaponIndex;
}


GunSwitch::~GunSwitch() = default;

void GunSwitch::nextWeapon() {
    m_currentWeaponIndex++;
    if (static_cast<size_t>(m_currentWeaponIndex) >= m_weaponRects.size()) {
        m_currentWeaponIndex = 0;
    }
    std::cout << "DEBUG: Am schimbat la arma: " << m_currentWeaponIndex << std::endl;
}

void GunSwitch::previousWeapon() {
    m_currentWeaponIndex--;
    if (m_currentWeaponIndex < 0) {
        m_currentWeaponIndex = static_cast<int>(m_weaponRects.size()) - 1;
    }
    std::cout << "DEBUG: Am schimbat la arma: " << m_currentWeaponIndex << std::endl;
}

sf::IntRect GunSwitch::getCurrentWeaponRect() const {
    return m_weaponRects[m_currentWeaponIndex];
}

std::ostream& operator<<(std::ostream& os, const GunSwitch& switcher) {
    os << "GunSwitch( Arma curenta:  " << switcher.m_currentWeaponIndex << " )";
    return os;
}