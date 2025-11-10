#include "Player.h"
#include "RandomGenerator.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <algorithm>

constexpr float PI = 3.14159265358979323846f;
static sf::SoundBuffer s_emptySoundBuffer;

Player::Player(float startX, float startY, const std::map<std::string, sf::SoundBuffer>& soundBuffers) : m_health(100.f),
    playerTexture(),
    playerSprite(this->playerTexture),
    m_gunSwitch(),
    m_isReloading(false),
    m_reloadAnimSprite(this->m_reloadAnimTexture),
    m_healthBarSprite(this->m_reloadAnimTexture),
    m_reloadingWeaponIndex(-1),
    m_isKnockedBack(false),
    m_knockbackDuration(0.15f),
    m_knockbackVelocity(0.f, 0.f),
    m_isInteracting(false),
    m_interactionTileID(0),
    m_interactionDuration(2.0f),
    m_wasMoving(false), m_coinCount(0),m_stepSound(s_emptySoundBuffer),m_stepCooldown(0.35f) {
    if (!this->playerTexture.loadFromFile("assets/Premium Content/Examples/Basic Usage.png")) {
        std::cerr << "EROARE: Nu am putut incarca ../assets/Premium Content/Examples/Basic Usage.png" << std::endl;
    }
    if (!this->bulletTexture.loadFromFile("assets/Bullets.png")) {
        std::cerr << "EROARE: Nu am putut incarca ../assets/Bullets.png" << std::endl;
    }
    if (!this->m_reloadAnimTexture.loadFromFile("assets/effects/All.png")) {
        std::cerr << "EROARE: Nu am putut incarca ../assets/effects/All.png" << std::endl;
    }

    if (soundBuffers.count("player_step")) {
        m_stepSound.setBuffer(soundBuffers.at("player_step"));
        m_stepSound.setVolume(100.f);
        std::cout << "DEBUG: Sunetul player_step a fost INCARCAT si atasat." << std::endl;
    } else {
        std::cerr << "EROARE FATALA: Nu am gasit 'player_step' in soundBuffers!" << std::endl;
    }

    m_healthBarSprite.setTexture(m_reloadAnimTexture);
    //---------------------------------------------------------------------------------------------------
    m_reloadAnimFrames.push_back(sf::IntRect({290, 162}, {44, 44}));
    m_reloadAnimFrames.push_back(sf::IntRect({338, 162}, {44, 44}));
    m_reloadAnimFrames.push_back(sf::IntRect({386, 162}, {44, 44}));
    m_reloadAnimFrames.push_back(sf::IntRect({434, 162}, {44, 44}));
    m_reloadAnimFrames.push_back(sf::IntRect({482, 162}, {44, 44}));

    //Numele armelor
    //---------------------------
    weaponNames.emplace_back("PISTOL");
    weaponNames.emplace_back("TOMMY GUN");
    weaponNames.emplace_back("RPG");
    weaponNames.emplace_back("SMG");
    weaponNames.emplace_back("SHOTGUN");
    weaponNames.emplace_back("SNIPER");
    //------------------------------

    //Sunete arme
    m_weaponShootSounds.emplace_back("pistol_shoot");
    m_weaponShootSounds.emplace_back("tommygun_shoot");
    m_weaponShootSounds.emplace_back("rpg_shoot");
    m_weaponShootSounds.emplace_back("smg_shoot");
    m_weaponShootSounds.emplace_back("shotgun_shoot");
    m_weaponShootSounds.emplace_back("sniper_shoot");

    m_weaponReloadSounds.emplace_back("pistol_reload");
    m_weaponReloadSounds.emplace_back("tommygun_reload");
    m_weaponReloadSounds.emplace_back("rpg_reload");
    m_weaponReloadSounds.emplace_back("smg_reload");
    m_weaponReloadSounds.emplace_back("shotgun_reload");
    m_weaponReloadSounds.emplace_back("sniper_reload");

    //Scalez animatia si incarc textura
    m_reloadAnimSprite.setScale(sf::Vector2f(1.4f, 1.4f));
    m_reloadAnimSprite.setTextureRect(m_reloadAnimFrames[0]);
    //---------------------------------------------------------------------------------------------------------

    m_weaponMaxReserveAmmo.push_back(70);
    m_weaponMaxReserveAmmo.push_back(300);
    m_weaponMaxReserveAmmo.push_back(12);
    m_weaponMaxReserveAmmo.push_back(180);
    m_weaponMaxReserveAmmo.push_back(32);
    m_weaponMaxReserveAmmo.push_back(20);

    // Pistol
    m_weaponBarrelOffsets.emplace_back(131.f, 356.f);
    std::vector<sf::IntRect> pistolFrames;
    pistolFrames.push_back(sf::IntRect({341, 22}, {6, 5}));
    pistolFrames.push_back(sf::IntRect({356, 22}, {7, 5}));
    pistolFrames.push_back(sf::IntRect({371, 22}, {8, 5}));
    pistolFrames.push_back(sf::IntRect({388, 22}, {7, 5}));
    m_movementSpeeds.emplace_back(400.f);
    m_weaponDamage.push_back(10.f);
    m_weaponBulletScales.emplace_back(4.f, 2.f);
    m_reloadAnimPosition.emplace_back(170.f, 640.f);
    m_weaponBulletAnimRects.push_back(pistolFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(0.5f);
    m_weaponReloadTime.emplace_back(0.5f);
    weaponMagSize.push_back(7);
    weaponCurrentAmmo.push_back(7);
    weaponReserveAmmo.push_back(70);

    // TommyGun
    m_weaponBarrelOffsets.emplace_back(115.f, 470.f);
    std::vector<sf::IntRect> tommyFrames;
    tommyFrames.push_back(sf::IntRect({256, 149}, {15, 6}));
    tommyFrames.push_back(sf::IntRect({273, 150}, {14, 4}));
    tommyFrames.push_back(sf::IntRect({290, 151}, {13, 3}));
    tommyFrames.push_back(sf::IntRect({305, 151}, {14, 4}));
    m_movementSpeeds.emplace_back(280.f);
    m_weaponDamage.push_back(35.f);
    m_weaponBulletScales.emplace_back(3.f, 1.f);
    m_reloadAnimPosition.emplace_back(210.f, 640.f);
    m_weaponBulletAnimRects.push_back(tommyFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(0.1f);
    m_weaponReloadTime.emplace_back(1.f);
    weaponMagSize.push_back(75);
    weaponCurrentAmmo.push_back(75);
    weaponReserveAmmo.push_back(300);

    // RPG
    m_weaponBarrelOffsets.emplace_back(80.f, 520.f);
    std::vector<sf::IntRect> rpgFrames;
    rpgFrames.push_back(sf::IntRect({454, 205}, {20, 7}));
    rpgFrames.push_back(sf::IntRect({484, 202}, {24, 13}));
    rpgFrames.push_back(sf::IntRect({514, 202}, {28, 13}));
    rpgFrames.push_back(sf::IntRect({544, 202}, {32, 13}));
    rpgFrames.push_back(sf::IntRect({575, 199}, {33, 17}));
    m_movementSpeeds.emplace_back(180.f);
    m_weaponDamage.push_back(90.f);
    m_weaponBulletScales.emplace_back(3.f, 3.f);
    m_reloadAnimPosition.emplace_back(140.f, 640.f);
    m_weaponBulletAnimRects.push_back(rpgFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(1.2f);
    m_weaponReloadTime.emplace_back(3.f);
    weaponMagSize.push_back(3);
    weaponCurrentAmmo.push_back(3);
    weaponReserveAmmo.push_back(12);

    // SMG
    m_weaponBarrelOffsets.emplace_back(100.f, 356.f);
    std::vector<sf::IntRect> smgFrames;
    smgFrames.push_back(sf::IntRect({181, 278}, {6, 4}));
    smgFrames.push_back(sf::IntRect({196, 277}, {8, 6}));
    smgFrames.push_back(sf::IntRect({208, 278}, {17, 5}));
    smgFrames.push_back(sf::IntRect({223, 278}, {17, 5}));
    m_movementSpeeds.emplace_back(350.f);
    m_weaponDamage.push_back(15.f);
    m_weaponBulletScales.emplace_back(3.f, 1.f);
    m_reloadAnimPosition.emplace_back(172.f, 640.f);
    m_weaponBulletAnimRects.push_back(smgFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(0.05f);
    m_weaponReloadTime.emplace_back(0.8f);
    weaponMagSize.push_back(35);
    weaponCurrentAmmo.push_back(35);
    weaponReserveAmmo.push_back(180);

    // Shotgun
    m_weaponBarrelOffsets.emplace_back(100.f, 460.f);
    std::vector<sf::IntRect> shotgunFrames;
    shotgunFrames.push_back(sf::IntRect({452, 264}, {20, 16}));
    shotgunFrames.push_back(sf::IntRect({484, 264}, {20, 15}));
    shotgunFrames.push_back(sf::IntRect({516, 264}, {23, 16}));
    shotgunFrames.push_back(sf::IntRect({546, 262}, {25, 19}));
    m_movementSpeeds.emplace_back(210.f);
    m_weaponDamage.push_back(70.f);
    m_weaponBulletScales.emplace_back(2.f, 3.f);
    m_reloadAnimPosition.emplace_back(170.f, 640.f);
    m_weaponBulletAnimRects.push_back(shotgunFrames);
    m_weaponBulletAnimSpeeds.push_back(0.05f);
    m_weaponShootCooldowns.push_back(1.f);
    m_weaponReloadTime.emplace_back(1.5f);
    weaponMagSize.push_back(8);
    weaponCurrentAmmo.push_back(8);
    weaponReserveAmmo.push_back(32);

    // Sniper
    m_weaponBarrelOffsets.emplace_back(107.f, 620.f);
    std::vector<sf::IntRect> sniperFrames;
    sniperFrames.push_back(sf::IntRect({0, 244}, {16, 9}));
    sniperFrames.push_back(sf::IntRect({14, 244}, {18, 9}));
    sniperFrames.push_back(sf::IntRect({31, 244}, {16, 9}));
    sniperFrames.push_back(sf::IntRect({46, 244}, {17, 9}));
    sniperFrames.push_back(sf::IntRect({63, 244}, {17, 9}));
    m_movementSpeeds.emplace_back(200.f);
    m_weaponDamage.push_back(150.f);
    m_weaponBulletScales.emplace_back(2.f, 1.f);
    m_reloadAnimPosition.emplace_back(170.f, 640.f);
    m_weaponBulletAnimRects.push_back(sniperFrames);
    m_weaponBulletAnimSpeeds.push_back(0.1f);
    m_weaponShootCooldowns.push_back(2.f);
    m_weaponReloadTime.emplace_back(2.f);
    weaponMagSize.push_back(5);
    weaponCurrentAmmo.push_back(5);
    weaponReserveAmmo.push_back(20);

    sf::IntRect skinRect = m_gunSwitch.getCurrentWeaponRect();
    this->playerSprite.setTextureRect(skinRect);

    //HealthBar
    m_healthBarSprite.setTexture(m_reloadAnimTexture);
    m_healthBarFrames.push_back(sf::IntRect({627, 6}, {42, 5}));
    m_healthBarFrames.push_back(sf::IntRect({675, 6}, {42, 5}));
    m_healthBarFrames.push_back(sf::IntRect({723, 6}, {42, 5}));
    m_healthBarFrames.push_back(sf::IntRect({771, 6}, {42, 5}));
    m_healthBarFrames.push_back(sf::IntRect({819, 6}, {42, 5}));
    m_healthBarFrames.push_back(sf::IntRect({867, 6}, {42, 5}));


    m_healthBarSprite.setTextureRect(m_healthBarFrames[0]);

    m_healthBarSprite.setScale({10.f, 5.f});
    updateHealthBar();
    updateHealthBarPosition();
    // -------------------------------

    this->playerSprite.scale({0.25f, 0.25f});
    this->playerSprite.setOrigin({
        static_cast<float>(skinRect.size.x) / 2.f, static_cast<float>(skinRect.size.y) / 2.f
    });
    this->playerSprite.setPosition({startX, startY});

    const float BAR_WIDTH = 200.f;
    const float BAR_HEIGHT = 10.f;

    m_progressBarBackground.setSize({BAR_WIDTH, BAR_HEIGHT});
    m_progressBarBackground.setFillColor(sf::Color(100, 100, 100, 150)); // Gri
    m_progressBarBackground.setOrigin({BAR_WIDTH / 2.f, BAR_HEIGHT / 2.f});

    m_progressBarFront.setSize({0, BAR_HEIGHT});
    m_progressBarFront.setFillColor(sf::Color(50, 255, 50, 200));
    m_progressBarFront.setOrigin({BAR_WIDTH / 2.f, BAR_HEIGHT / 2.f});
}

Player::~Player() = default;

void Player::drawWorld(sf::RenderWindow &window) const {
    window.draw(this->playerSprite);
}

void Player::drawUI(sf::RenderWindow &window) {
    if (m_isReloading) {
        window.draw(m_reloadAnimSprite);
    }
    window.draw(this->m_healthBarSprite);

    if (m_isInteracting) {
        sf::Vector2f viewCenter = window.getView().getCenter();
        sf::Vector2f barPos = {viewCenter.x, viewCenter.y + 300.f};

        m_progressBarBackground.setPosition(barPos);
        m_progressBarFront.setPosition(barPos);

        window.draw(m_progressBarBackground);
        window.draw(m_progressBarFront);
    }
}

sf::FloatRect Player::getCollisionBounds() const {
    float boxWidth = 15.f;
    float boxHeight = 25.f;
    sf::Vector2f pos = getPosition();
    return {
        {pos.x - boxWidth / 2.f, pos.y - boxHeight / 2.f},
        {boxWidth, boxHeight}
    };
}

void Player::takeDamage(float damage,sf::Vector2f knockbackDirection) {
    if (m_isKnockedBack)return;

    m_isInteracting = false;
    m_isReloading = false;

    this->m_health.takeDamage(damage);
    updateHealthBar();

    m_isKnockedBack = true;
    m_knockbackTimer.restart();

    const float knockbackSpeed = 400.f;
    m_knockbackVelocity = knockbackSpeed * knockbackDirection;
}

void Player::addHealth(float amount)
{
    m_health.addHealth(amount);
    updateHealthBar();
}

void Player::addAmmo(int amount)
{
    int currentIndex = m_gunSwitch.getCurrentWeaponIndex();
    if (currentIndex >= 0 && static_cast<size_t>(currentIndex) < weaponReserveAmmo.size())
    {
        int maxReserve = m_weaponMaxReserveAmmo[currentIndex];
        weaponReserveAmmo[currentIndex] = std::min(weaponReserveAmmo[currentIndex] + amount, maxReserve);
    }
}

//Updateaza healthbarul
void Player::updateHealthBar() {
    float healthPercent = m_health.getPercentage();
    int frameCount = static_cast<int>(m_healthBarFrames.size());
    int frameIndex = static_cast<int>(std::ceil((1.f - healthPercent) * static_cast<float>(frameCount - 1)));
    if (frameIndex < 0)
        frameIndex = 0;
    if (frameIndex >= frameCount) frameIndex = frameCount - 1;
    m_healthBarSprite.setTextureRect(m_healthBarFrames[frameIndex]);
}

//Update la pozitia healthbarului
void Player::updateHealthBarPosition() {
    float barWidth = m_healthBarSprite.getGlobalBounds().size.x;
    float x = (1280.f / 2.f) - (barWidth / 2.f);
    float y = 10.f;
    m_healthBarSprite.setPosition({x, y});
}

float Player::getCurrentWeaponCooldown() const {
    int index = m_gunSwitch.getCurrentWeaponIndex();
    if (index < 0 || static_cast<size_t>(index) >= m_weaponShootCooldowns.size()) {
        return 0.3f;
    }
    return m_weaponShootCooldowns[index];
}

void Player::update(float dt, sf::Vector2f mousePosition, GameMap &gameMap) {
    if (m_isInteracting)
    {
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X))
        {
            m_isInteracting = false;
            return;
        }
        float elapsed = m_interactionTimer.getElapsedTime().asSeconds();
        float progress = elapsed / m_interactionDuration;

        float barWidth = m_progressBarBackground.getSize().x;
        m_progressBarFront.setSize({barWidth * progress, m_progressBarFront.getSize().y});

        if (elapsed >= m_interactionDuration)
        {
            m_isInteracting = false;
            sf::Vector2u tileCoords = gameMap.getTileCoordsAt(getPosition());
            if (m_interactionTileID == 71) {
                addHealth(50);
                gameMap.startPickupCooldown(tileCoords);
            } else if (m_interactionTileID == 72) {
                addAmmo(20);
                gameMap.startPickupCooldown(tileCoords);
            }
        }
        return;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X) && !m_isReloading && !m_isKnockedBack) {
        sf::Vector2f playerCenter = getPosition();

        sf::Vector2u tileCoords = gameMap.getTileCoordsAt(playerCenter);
        const int tileID = gameMap.getTileIDAt(playerCenter);

        if (gameMap.isPickupOnCooldown(tileCoords)) {
            m_isInteracting = false;
        }else {
            bool canInteract = false;
            if (tileID == 71) {
                if (!m_health.isFull()) {
                    canInteract = true;
                }
            }else if (tileID == 72) {
                int currentIndex = m_gunSwitch.getCurrentWeaponIndex();
                if (currentIndex >= 0 && static_cast<size_t>(currentIndex) < weaponReserveAmmo.size()) {
                    if (weaponReserveAmmo[currentIndex] < m_weaponMaxReserveAmmo[currentIndex]) {
                        canInteract = true;
                    }
                }
            }

            if (canInteract) {
                m_isInteracting = true;
                m_interactionTimer.restart();
                m_interactionTileID = tileID;
                sf::Vector2f tileCenter = gameMap.getTileCenter(playerCenter);
                this->playerSprite.setPosition(tileCenter);
                return;
            }
        }
    }

    sf::Vector2f velocity;

    if (m_isKnockedBack)
    {
        if (m_knockbackTimer.getElapsedTime().asSeconds() > m_knockbackDuration)
        {
            m_isKnockedBack = false;
        } else {
            velocity = m_knockbackVelocity;
        }
    }
    if (!m_isKnockedBack)
    {
        sf::Vector2f direction(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) direction.y = -1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) direction.y = 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) direction.x = -1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) direction.x = 1.f;

        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length != 0.f) {
            direction /= length;
        }

        bool isMoving = (direction.x != 0.f || direction.y != 0.f);

        if (isMoving) {
            if (!m_wasMoving) {
                std::cout << "DEBUG: Primul pas!" << std::endl;
                m_stepSound.setPitch(RandomGenerator::getFloat(0.9f, 1.1f));
                m_stepSound.play();
                m_stepTimer.restart();
            } else {
                if (m_stepTimer.getElapsedTime().asSeconds() > m_stepCooldown) {
                    std::cout << "DEBUG: Pas urmator (dupa cooldown)!" << std::endl;
                    m_stepSound.setPitch(RandomGenerator::getFloat(0.9f, 1.1f));
                    m_stepSound.play();
                    m_stepTimer.restart();
                }
            }
        }
        m_wasMoving = isMoving;

        int currentSpeedIndex = m_gunSwitch.getCurrentWeaponIndex();
        float currentSpeed = 270.f;
        if (currentSpeedIndex >= 0 && static_cast<size_t>(currentSpeedIndex) < m_movementSpeeds.size()) {
            currentSpeed = m_movementSpeeds[currentSpeedIndex];
        }
        const float baseSpeedForSteps = 300.f;
        const float baseStepCooldown = 0.4f;

        if (currentSpeed > 0.f) {
            m_stepCooldown = baseStepCooldown * (baseSpeedForSteps / currentSpeed);
        } else {
            m_stepCooldown = baseStepCooldown;
        }
        velocity = direction * currentSpeed;
    }

    sf::Vector2f currentPos = getPosition();
    sf::Vector2f frameVelocity = velocity * dt;


    sf::FloatRect bounds = getCollisionBounds();
    bounds.position.x += frameVelocity.x;

    float topY = bounds.position.y;
    float bottomY = bounds.position.y + bounds.size.y;

    if (velocity.x > 0) {
        float rightX = bounds.position.x + bounds.size.x;
        if (gameMap.isSolid({rightX, topY}) || gameMap.isSolid({rightX, bottomY})) {
            frameVelocity.x = 0;
        }
    } else if (velocity.x < 0) {
        float leftX = bounds.position.x;
        if (gameMap.isSolid({leftX, topY}) || gameMap.isSolid({leftX, bottomY})) {
            frameVelocity.x = 0;
        }
    }

    currentPos.x += frameVelocity.x;
    this->playerSprite.setPosition({currentPos.x, currentPos.y});

    bounds = getCollisionBounds();
    bounds.position.y += frameVelocity.y;

    float leftX = bounds.position.x;
    float rightX = bounds.position.x + bounds.size.x;

    if (velocity.y > 0) {
        float bottomY_check = bounds.position.y + bounds.size.y;
        if (gameMap.isSolid({leftX, bottomY_check}) || gameMap.isSolid({rightX, bottomY_check})) {
            frameVelocity.y = 0;
        }
    } else if (velocity.y < 0) {
        float topY_check = bounds.position.y;
        if (gameMap.isSolid({leftX, topY_check}) || gameMap.isSolid({rightX, topY_check})) {
            frameVelocity.y = 0;
        }
    }

    currentPos.y += frameVelocity.y;
    this->playerSprite.setPosition({currentPos.x, currentPos.y});

    sf::Vector2f playerPosition = this->playerSprite.getPosition();
    float deltaX = mousePosition.x - playerPosition.x;
    float deltaY = mousePosition.y - playerPosition.y;
    float angleInRadians = std::atan2(deltaY, deltaX);
    float angleInDegrees = angleInRadians * (180.f / PI);

    this->playerSprite.setRotation(sf::degrees(angleInDegrees - 90.f));

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

std::ostream &operator<<(std::ostream &os, const Player &player) {
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

//Functia pentru tras(shooting)
Bullet Player::shoot(sf::Vector2f mousePosition) {
    int currentIndex = m_gunSwitch.getCurrentWeaponIndex();
    if (currentIndex >= 0 && static_cast<size_t>(currentIndex) < weaponCurrentAmmo.size()) {
        weaponCurrentAmmo[currentIndex]--;
    }

    if (currentIndex < 0 || static_cast<size_t>(currentIndex) >= m_weaponBulletAnimRects.size()) {
        currentIndex = 0;
    }

    const std::vector<sf::IntRect> &bulletAnimRects = m_weaponBulletAnimRects[currentIndex];

    sf::Vector2f barrelOffset = m_weaponBarrelOffsets[currentIndex];
    float animSpeed = m_weaponBulletAnimSpeeds[currentIndex];

    sf::Vector2f bulletScale = m_weaponBulletScales[currentIndex];

    float currentDamage = m_weaponDamage[currentIndex];
    const float localBarrelOffsetX = barrelOffset.x;
    const float localBarrelOffsetY = barrelOffset.y;

    sf::Vector2f localBarrelPos(localBarrelOffsetX, localBarrelOffsetY);
    sf::Transform playerTransform = playerSprite.getTransform();

    sf::Vector2f barrelPosition = playerTransform.transformPoint(localBarrelPos);

    sf::Vector2f direction = mousePosition - barrelPosition;

    return {
        bulletTexture, bulletAnimRects, barrelPosition, direction,
        animSpeed, bulletScale,currentDamage
    };
}

bool Player::canShoot(sf::Vector2f mousePosition) const {
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

std::string Player::getShootSoundKey() const {
    int index = m_gunSwitch.getCurrentWeaponIndex();
    if (index < 0 || static_cast<size_t>(index) >= m_weaponShootSounds.size()) {
        return "";
    }
    return m_weaponShootSounds[index];
}

std::pair<std::string,float> Player::reload() {
    if (m_isReloading) {
        return {"", 0.f};
    }

    int index = m_gunSwitch.getCurrentWeaponIndex();

    if (index < 0 ||
        static_cast<size_t>(index) >= m_weaponReloadTime.size() ||
        static_cast<size_t>(index) >= weaponMagSize.size() ||
        static_cast<size_t>(index) >= weaponCurrentAmmo.size() ||
        static_cast<size_t>(index) >= weaponReserveAmmo.size()) {
        std::cerr << "Eroare: Date de reincarcare invalide pentru indexul " << index << std::endl;
        return {"", 0.f};
    }

    int magSize = weaponMagSize[index];
    int currentAmmo = weaponCurrentAmmo[index];
    int reserveAmmo = weaponReserveAmmo[index];

    int amountNeeded = magSize - currentAmmo;
    if (amountNeeded <= 0 || reserveAmmo <= 0) {
        return {"", 0.f};
    }

    if (static_cast<size_t>(index) >= m_reloadAnimPosition.size()) {
        std::cerr << "Eroare: Pozitie de reincarcare invalida pentru indexul " << index << std::endl;
        return {"", 0.f};
    }
    m_reloadAnimSprite.setPosition(m_reloadAnimPosition[index]);

    m_isReloading = true;
    m_reloadTimer.restart();
    m_reloadingWeaponIndex = index;
    m_currentReloadTotalTime = m_weaponReloadTime[index];

    std::string soundKeyToPlay = m_weaponReloadSounds[index];
    std::cout << "Reloading... (va dura " << m_currentReloadTotalTime << "s)" << std::endl;

    return {soundKeyToPlay, m_currentReloadTotalTime};
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

std::string Player::getCurrentWeaponName() const {
    int index = m_gunSwitch.getCurrentWeaponIndex();
    if (index < 0 || static_cast<size_t>(index) >= weaponNames.size())
        return "Unknown";
    return weaponNames[index];
}

void Player::addCoins(int amount) {
    if (amount>0) {
        m_coinCount+=amount;
        std::cout << "Coins added: " << amount << std::endl;
    }
}

int Player::getCoinCount() const {
    return m_coinCount;
}