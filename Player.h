#pragma once
#include <SFML/Graphics.hpp>
#include "Health.h"
#include "GunSwitch.h"
#include "Bullet.h"

class Player {
public:
    Player(float startX, float startY);
    ~Player();

    //Functiile triviale ale playerului
    void update(float dt, sf::Vector2f mousePosition);
    void draw(sf::RenderWindow& window) const;
    void drawUI(sf::RenderWindow& window);
    void takeDamage(float damage);
    sf::Vector2f getPosition() const;

    //Functii triviale pentru uzul armelor
    void switchWeaponNext();
    void switchWeaponPrev();
    Bullet shoot(sf::Vector2f mousePosition);
    float getCurrentWeaponCooldown() const;
    int getCurrentAmmo() const;
    int getReserveAmmo() const;
    std::string getCurrentWeaponName() const;
    void reload();
    bool canShoot(sf::Vector2f mousePosition) const;

    friend std::ostream& operator<<(std::ostream& os, const Player& player);

private:
    //Healthbar
    void updateHealthBar();
    void updateHealthBarPosition();
    sf::RectangleShape HealthBarBackground;
    sf::RectangleShape HealthBarForeground;
    Health m_health;

    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    std::vector<float> m_movementSpeeds;

    //Sectiune pentru arme,gloante,vectori pentru a modifica usor valori ale armelor din Player.cpp
    //----------------------------------------
    GunSwitch m_gunSwitch;
    sf::Texture bulletTexture;
    std::vector<sf::Vector2f> m_weaponBarrelOffsets;
    std::vector<sf::IntRect> m_weaponBulletRects;
    std::vector<float> m_weaponBulletAnimSpeeds;
    std::vector<float> m_weaponShootCooldowns;
    std::vector<std::vector<sf::IntRect>> m_weaponBulletAnimRects;
    std::vector<int> weaponMagSize;
    std::vector<int> weaponCurrentAmmo;
    std::vector<int> weaponReserveAmmo;
    std::vector<std::string> weaponNames;
    std::vector<float> m_weaponReloadTime;
    bool m_isReloading;
    sf::Clock m_reloadTimer;
    sf::Texture m_reloadAnimTexture;
    sf::Sprite m_reloadAnimSprite;
    std::vector<sf::IntRect> m_reloadAnimFrames;
    std::vector<sf::Vector2f> m_reloadAnimPosition;
    int m_reloadingWeaponIndex;
    float m_currentReloadTotalTime{};
    //--------------------------------------------------
};