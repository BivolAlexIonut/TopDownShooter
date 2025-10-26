#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class GunSwitch {
public:
    GunSwitch();
    ~GunSwitch();

    void nextWeapon();
    void previousWeapon();
    [[nodiscard]] sf::IntRect getCurrentWeaponRect() const;
    [[nodiscard]] int getCurrentWeaponIndex() const;

    friend std::ostream& operator<<(std::ostream& os, const GunSwitch& switcher);

private:
    int m_currentWeaponIndex;
    std::vector<sf::IntRect> m_weaponRects;
};