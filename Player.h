#pragma once
#ifndef __clangd__
#include <SFML/Graphics.hpp>
#endif
#include <SFML/Audio.hpp>
#include <utility>
#include <string>
#include<map>
#include "Health.h"
#include "GunSwitch.h"
#include "Bullet.h"
#include "GameMap.h"

class Player {
public:
    Player(float startX, float startY,const std::map<std::string, sf::SoundBuffer>& soundBuffers);
    ~Player();

    //Functiile triviale ale playerului
    void update(float dt, sf::Vector2f mousePosition,GameMap& gameMap);
    sf::FloatRect getCollisionBounds() const;

    void drawWorld(sf::RenderWindow& window) const;
    void drawUI(sf::RenderWindow& window);
    void takeDamage(float damage,sf::Vector2f knockbackDirection);
    sf::Vector2f getPosition() const;

    //------------------
    void addHealth(float amount);
    void addAmmo(int amount);
    //-------------------

    void switchWeaponNext();
    void switchWeaponPrev();
    Bullet shoot(sf::Vector2f mousePosition);
    float getCurrentWeaponCooldown() const;
    int getCurrentAmmo() const;
    int getReserveAmmo() const;
    std::string getCurrentWeaponName() const;
    bool canShoot(sf::Vector2f mousePosition) const;

    void addCoins(int amount);
    [[nodiscard]] int getCoinCount() const;

    std::string getShootSoundKey() const;
    std::pair<std::string, float> reload();

    std::string upgradeCurrentWeaponDamage();
    std::string upgradeCurrentWeaponFireRate();
    std::string upgradeMaxHealth();
    std::string upgradeCurrentWeaponMoveSpeed();
    std::map<std::string, std::string> getUpgradeInfo() const;
    bool spendCoins(int amount);

    friend std::ostream& operator<<(std::ostream& os, const Player& player);

private:
    void updateHealthBar();
    void updateHealthBarPosition();
    std::vector<sf::IntRect> m_healthBarFrames;

    Health m_health;

    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    std::vector<float> m_movementSpeeds;

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
    sf::Sprite m_healthBarSprite;
    std::vector<sf::IntRect> m_reloadAnimFrames;
    std::vector<sf::Vector2f> m_reloadAnimPosition;
    int m_reloadingWeaponIndex;
    float m_currentReloadTotalTime{};

    std::vector<sf::Vector2f> m_weaponBulletScales;

    bool m_isKnockedBack;
    float m_knockbackDuration;
    sf::Vector2f m_knockbackVelocity;
    sf::Clock m_knockbackTimer;

    std::vector<float> m_weaponDamage;

    bool m_isInteracting;
    int m_interactionTileID;
    float m_interactionDuration;
    sf::Clock m_interactionTimer;
    std::vector<int> m_weaponMaxReserveAmmo;

    sf::RectangleShape m_progressBarBackground;
    sf::RectangleShape m_progressBarFront;

    bool m_wasMoving;

    int m_coinCount;
    sf::Sound m_stepSound;
    sf::Clock m_stepTimer;
    float m_stepCooldown = 0.20f;

    std::vector<std::string> m_weaponShootSounds;
    std::vector<std::string> m_weaponReloadSounds;

    std::vector<int> m_upgradeCostDamage;
    std::vector<int> m_upgradeCostFireRate;
    std::vector<int> m_upgradeCostMoveSpeed;
    int m_upgradeCostHealth;
    //--------------------------------------------------
};
