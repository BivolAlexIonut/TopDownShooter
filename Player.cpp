// Player.cpp
#include "Player.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <algorithm>

const float PI = 3.14159265358979323846f;
const float HEALTHBAR_WIDTH = 300.f;
const float HEALTHBAR_HEIGHT = 7.f;
const float HEALTHBAR_OFFSET_Y = -360.f;

Player::Player(float startX, float startY) :
    playerTexture(),
    playerSprite(this->playerTexture),
    m_health(100.f),
    m_gunSwitch(),
    m_isReloading(false),//Incep prin a nu reincarca
    m_reloadAnimSprite(this->m_reloadAnimTexture),
    m_reloadingWeaponIndex(-1)
{
    if (!this->playerTexture.loadFromFile("../assets/Premium Content/Examples/Basic Usage.png")) {
        std::cerr << "EROARE: Nu am putut incarca ../assets/Premium Content/Examples/Basic Usage.png" << std::endl;
    }
    if (!this->bulletTexture.loadFromFile("../assets/Bullets.png")) {
        std::cerr <<"EROARE: Nu am putut incarca ../assets/Bullets.png" << std::endl;
    }
    if (!this->m_reloadAnimTexture.loadFromFile("../assets/effects/All.png")) {
        std::cerr<<"EROARE: Nu am putut incarca ../assets/effects/All.png"<<std::endl;
    }
    //---------------------------------------------------------------------------------------------------
    m_reloadAnimFrames.push_back(sf::IntRect({290, 162}, {44, 44}));
    m_reloadAnimFrames.push_back(sf::IntRect({338, 162}, {44, 44}));
    m_reloadAnimFrames.push_back(sf::IntRect({386, 162}, {44, 44}));
    m_reloadAnimFrames.push_back(sf::IntRect({434, 162}, {44, 44}));
    m_reloadAnimFrames.push_back(sf::IntRect({482, 162}, {44, 44}));

    //Unde apare aniamtia pe ecran
    m_reloadAnimPosition = sf::Vector2f(210.f, 650.f);
    m_reloadAnimSprite.setScale(sf::Vector2f(1.4f, 1.4f));
    m_reloadAnimSprite.setPosition(m_reloadAnimPosition);

    if (!m_reloadAnimFrames.empty()) {
        m_reloadAnimSprite.setTextureRect(m_reloadAnimFrames[0]);
    }
    //---------------------------------------------------------------------------------------------------------

    // Pistol
    m_weaponBarrelOffsets.emplace_back(131.f, 356.f);
    std::vector<sf::IntRect> pistolFrames;
    pistolFrames.push_back(sf::IntRect({341, 22}, {6, 5}));
    pistolFrames.push_back(sf::IntRect({356, 22}, {7, 5}));
    pistolFrames.push_back(sf::IntRect({371, 22}, {8, 5}));
    pistolFrames.push_back(sf::IntRect({388, 22}, {7, 5}));
    m_weaponBulletAnimRects.push_back(pistolFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(0.5f);
    m_weaponReloadTime.emplace_back(0.5f);
    weaponMagSize.push_back(7);
    weaponCurrentAmmo.push_back(7);
    weaponReserveAmmo.push_back(35);

    // TommyGun
    m_weaponBarrelOffsets.emplace_back(115.f, 400.f);
    std::vector<sf::IntRect> tommyFrames;
    tommyFrames.push_back(sf::IntRect({256, 149}, {15, 6}));
    tommyFrames.push_back(sf::IntRect({273, 150}, {14, 4}));
    tommyFrames.push_back(sf::IntRect({290, 151}, {13, 3}));
    tommyFrames.push_back(sf::IntRect({305, 151}, {14, 4}));
    m_weaponBulletAnimRects.push_back(tommyFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(0.08f);
    m_weaponReloadTime.emplace_back(1.f);
    weaponMagSize.push_back(75);
    weaponCurrentAmmo.push_back(75);
    weaponReserveAmmo.push_back(150);

    // RPG
    m_weaponBarrelOffsets.emplace_back(100.f, 470.f);
    std::vector<sf::IntRect> rpgFrames;
    rpgFrames.push_back(sf::IntRect({454, 205}, {20, 7}));
    rpgFrames.push_back(sf::IntRect({484, 202}, {24, 13}));
    rpgFrames.push_back(sf::IntRect({514, 202}, {28, 13}));
    rpgFrames.push_back(sf::IntRect({544, 202}, {32, 13}));
    rpgFrames.push_back(sf::IntRect({575, 199}, {33, 17}));
    m_weaponBulletAnimRects.push_back(rpgFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(1.2f);
    m_weaponReloadTime.emplace_back(3.f);
    weaponMagSize.push_back(3);
    weaponCurrentAmmo.push_back(3);
    weaponReserveAmmo.push_back(9);

    // SMG
    m_weaponBarrelOffsets.emplace_back(100.f, 356.f);
    std::vector<sf::IntRect> smgFrames;
    smgFrames.push_back(sf::IntRect({181, 278}, {6, 4})); // Cadru 1
    smgFrames.push_back(sf::IntRect({196, 277}, {8, 6})); // Cadru 2 (EXEMPLU!)
    smgFrames.push_back(sf::IntRect({208, 278}, {17, 5})); // Cadru 3 (EXEMPLU!)
    smgFrames.push_back(sf::IntRect({223, 278}, {17, 5})); // Cadru 4 (EXEMPLU!)
    m_weaponBulletAnimRects.push_back(smgFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(0.02f);
    m_weaponReloadTime.emplace_back(0.8f);
    weaponMagSize.push_back(35);
    weaponCurrentAmmo.push_back(35);
    weaponReserveAmmo.push_back(120);

    // Shotgun
    m_weaponBarrelOffsets.emplace_back(100.f, 400.f);
    std::vector<sf::IntRect> shotgunFrames;
    shotgunFrames.push_back(sf::IntRect({452, 264}, {20, 16})); // Cadru 1
    shotgunFrames.push_back(sf::IntRect({484, 264}, {20, 15})); // Cadru 2 (EXEMPLU!)
    shotgunFrames.push_back(sf::IntRect({516, 264}, {23, 16})); // Cadru 3 (EXEMPLU!)
    shotgunFrames.push_back(sf::IntRect({546, 262}, {25, 19})); // Cadru 4 (EXEMPLU!)
    m_weaponBulletAnimRects.push_back(shotgunFrames);
    m_weaponBulletAnimSpeeds.push_back(0.05f);
    m_weaponShootCooldowns.push_back(1.f);
    m_weaponReloadTime.emplace_back(1.5f);
    weaponMagSize.push_back(8);
    weaponCurrentAmmo.push_back(8);
    weaponReserveAmmo.push_back(16);

    // Sniper
    m_weaponBarrelOffsets.emplace_back(107.f, 470.f);
    std::vector<sf::IntRect> sniperFrames;
    sniperFrames.push_back(sf::IntRect({0, 244}, {16, 9}));
    sniperFrames.push_back(sf::IntRect({14, 244}, {18, 9}));
    sniperFrames.push_back(sf::IntRect({31, 244}, {16, 9}));
    sniperFrames.push_back(sf::IntRect({46, 244}, {17, 9}));
    sniperFrames.push_back(sf::IntRect({63, 244}, {17, 9}));
    m_weaponBulletAnimRects.push_back(sniperFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(2.f);
    m_weaponReloadTime.emplace_back(2.f);
    weaponMagSize.push_back(5);
    weaponCurrentAmmo.push_back(5);
    weaponReserveAmmo.push_back(10);

    sf::IntRect skinRect = m_gunSwitch.getCurrentWeaponRect();
    this->playerSprite.setTextureRect(skinRect);

    //HealthBar
    HealthBarBackground.setSize(sf::Vector2f(HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT));
    HealthBarBackground.setFillColor(sf::Color(50,50,50));
    HealthBarForeground.setSize(sf::Vector2f(HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT));
    HealthBarForeground.setFillColor(sf::Color::Green);
    updateHealthBar();
    updateHealthBarPosition();

    this->playerSprite.scale({0.2f,0.2f});
    this->playerSprite.setOrigin({static_cast<float>(skinRect.size.x) / 2.f, static_cast<float>(skinRect.size.y) / 2.f});
    this->playerSprite.setPosition({startX, startY});
    this->movementSpeed = 270.f;
}

Player::~Player() = default;

void Player::draw(sf::RenderWindow& window) const {
    window.draw(this->playerSprite);
    window.draw(this->HealthBarBackground);
    window.draw(this->HealthBarForeground);
}

void Player::drawUI(sf::RenderWindow& window) {
    if (m_isReloading) {
        window.draw(m_reloadAnimSprite);
    }
}

void Player::takeDamage(float damage) {
    this->m_health.takeDamage(damage);
    updateHealthBar();
}

void Player::updateHealthBar() {
    float healthpercent = m_health.getPercentage();
    float netWidth = HEALTHBAR_WIDTH * healthpercent;
    this->HealthBarForeground.setSize(sf::Vector2f(netWidth,HEALTHBAR_HEIGHT));
    updateHealthBarPosition();
}

float Player::getCurrentWeaponCooldown() const {
    int index = m_gunSwitch.getCurrentWeaponIndex();
    if (index < 0 || static_cast<size_t>(index) >= m_weaponShootCooldowns.size()) {
        return 0.3f;
    }
    return m_weaponShootCooldowns[index];
}

void Player::updateHealthBarPosition() {
    sf::Vector2f playerPos = this->playerSprite.getPosition();
    float x = playerPos.x - (HEALTHBAR_WIDTH/2);
    float y = playerPos.y + HEALTHBAR_OFFSET_Y;
    this->HealthBarBackground.setPosition({x,y});
    this->HealthBarForeground.setPosition({x,y});
}

void Player::update(float dt, sf::Vector2f mousePosition) {
    sf::Vector2f direction(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) direction.y = -1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) direction.y = 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) direction.x = -1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) direction.x = 1.f;

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0.f) {
        direction /= length;
    }
    this->playerSprite.move(direction * this->movementSpeed * dt);

    sf::Vector2f playerPosition = this->playerSprite.getPosition();
    float deltaX = mousePosition.x - playerPosition.x;
    float deltaY = mousePosition.y - playerPosition.y;
    float angleInRadians = std::atan2(deltaY, deltaX);
    float angleInDegrees = angleInRadians * (180.f / PI);

    this->playerSprite.setRotation(sf::degrees(angleInDegrees - 90.f));
    updateHealthBarPosition();

    if (m_isReloading) {
        if (m_reloadingWeaponIndex < 0 || m_reloadAnimFrames.empty()) {
            m_isReloading = false;
            return;
        }

        float elapsedTime = m_reloadTimer.getElapsedTime().asSeconds();
        int frameCount = static_cast<int>(m_reloadAnimFrames.size());

        float reloadProgress = elapsedTime / m_currentReloadTotalTime;
        int currentFrameIndex = static_cast<int>(reloadProgress * static_cast<float>(frameCount));
        if (currentFrameIndex >= frameCount) {
            currentFrameIndex = frameCount - 1;
        }
        m_reloadAnimSprite.setTextureRect(m_reloadAnimFrames[currentFrameIndex]);

        if (elapsedTime >= m_currentReloadTotalTime) {

            m_isReloading = false;
            int index = m_reloadingWeaponIndex;

            int magSize = weaponMagSize[index];
            int currentAmmo = weaponCurrentAmmo[index];
            int &reserveAmmo = weaponReserveAmmo[index];
            int amountNeeded = magSize - currentAmmo;

            int ammoToMove = std::min(amountNeeded, reserveAmmo);
            weaponCurrentAmmo[index] += ammoToMove;
            reserveAmmo -= ammoToMove;

            //debug
            std::cout << "Reloaded complete! Ammo: " << weaponCurrentAmmo[index] << "/" << reserveAmmo << std::endl;

            m_reloadingWeaponIndex = -1;
        }
    }
}

sf::Vector2f Player::getPosition() const {
    return this->playerSprite.getPosition();
}

std::ostream& operator<<(std::ostream& os, const Player& player) {
    sf::Vector2f pos = player.getPosition();
    os << "Player( Poz: " << pos.x << ", " << pos.y << " | " << player.m_health << " | " << player.m_gunSwitch << ")";
    return os;
}

void Player::switchWeaponNext() {
    if (m_isReloading)return;

    m_gunSwitch.nextWeapon();
    sf::IntRect newRect = m_gunSwitch.getCurrentWeaponRect();
    this->playerSprite.setTextureRect(newRect);
    this->playerSprite.setOrigin({static_cast<float>(newRect.size.x) / 2.f, static_cast<float>(newRect.size.y) / 2.f});
}

void Player::switchWeaponPrev() {
    if (m_isReloading)return;

    m_gunSwitch.previousWeapon();
    sf::IntRect newRect = m_gunSwitch.getCurrentWeaponRect();
    this->playerSprite.setTextureRect(newRect);
    this->playerSprite.setOrigin({static_cast<float>(newRect.size.x) / 2.f, static_cast<float>(newRect.size.y) / 2.f});
}


Bullet Player::shoot(sf::Vector2f mousePosition) {
    int currentIndex = m_gunSwitch.getCurrentWeaponIndex();
    if (currentIndex >= 0 && static_cast<size_t>(currentIndex) < weaponCurrentAmmo.size() ) {
        weaponCurrentAmmo[currentIndex] --;
    }

    if (currentIndex < 0 || static_cast<size_t>(currentIndex) >= m_weaponBulletAnimRects.size()) {
        currentIndex = 0;
    }

    const std::vector<sf::IntRect>& bulletAnimRects = m_weaponBulletAnimRects[currentIndex];

    sf::Vector2f barrelOffset = m_weaponBarrelOffsets[currentIndex];
    float animSpeed = m_weaponBulletAnimSpeeds[currentIndex];

    const float localBarrelOffsetX = barrelOffset.x;
    const float localBarrelOffsetY = barrelOffset.y;

    sf::Vector2f localBarrelPos(localBarrelOffsetX, localBarrelOffsetY);
    sf::Transform playerTransform = playerSprite.getTransform();

    sf::Vector2f barrelPosition = playerTransform.transformPoint(localBarrelPos);

    sf::Vector2f direction = mousePosition - barrelPosition;

    return {bulletTexture, bulletAnimRects, barrelPosition, direction,
        animSpeed};
}

bool Player::canShoot(sf::Vector2f mousePosition) const{
    if (m_isReloading) {
        return false;
    }

    if (this->playerSprite.getGlobalBounds().contains(mousePosition)) {
        return false;
    }

    int index = m_gunSwitch.getCurrentWeaponIndex();
    if (index < 0 || static_cast<size_t>(index) >= weaponCurrentAmmo.size())
        return false;
    return weaponCurrentAmmo[index] > 0;
}

bool Player::isReloading() const {
    return m_isReloading;
}


void Player::reload() {
    if (m_isReloading) {
        return;
    }

    int index = m_gunSwitch.getCurrentWeaponIndex();

    if (index < 0 ||
    static_cast<size_t>(index) >= m_weaponReloadTime.size() ||
    static_cast<size_t>(index) >= weaponMagSize.size() ||
    static_cast<size_t>(index) >= weaponCurrentAmmo.size() ||
    static_cast<size_t>(index) >= weaponReserveAmmo.size())
    {
        std::cerr << "Eroare: Date de reincarcare invalide pentru indexul " << index << std::endl;
        return;
    }

    int magSize = weaponMagSize[index];
    int currentAmmo = weaponCurrentAmmo[index];
    int reserveAmmo = weaponReserveAmmo[index];

    int amountNeeded = magSize - currentAmmo;
    if (amountNeeded <= 0 || reserveAmmo <= 0) {
        return;
    }

    m_isReloading = true;
    m_reloadTimer.restart();
    m_reloadingWeaponIndex = index;
    m_currentReloadTotalTime = m_weaponReloadTime[index];

    std::cout << "Reloading... (va dura " << m_currentReloadTotalTime << "s)" << std::endl;
}

int Player::getCurrentAmmo() const {
    int index = m_gunSwitch.getCurrentWeaponIndex();
    if (index < 0 || static_cast<size_t>(index) >= weaponCurrentAmmo.size())return 0;
    return weaponCurrentAmmo[index];
}

int Player::getReserveAmmo() const {
    int index = m_gunSwitch.getCurrentWeaponIndex();
    if (index < 0 || static_cast<size_t>(index) >= weaponReserveAmmo.size()) return 0;
    return weaponReserveAmmo[index];
}