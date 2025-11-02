#include <cmath>
#include <iostream>
#include <ostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "ChaserEnemy.h"
#include "GameMap.h"
#include "Player.h"
#include "RandomGenerator.h"
#include "Effect.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Top-Down Shooter");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);
    sf::Texture crosshairTexture;
    if (!crosshairTexture.loadFromFile("assets/ui/crosshair.png")) {
        std::cerr << "NU AM PUTUT INCARCA ui/crosshair.png" << std::endl;
    }
    sf::Sprite crosshairSprite(crosshairTexture);
    crosshairSprite.setTextureRect(sf::IntRect({static_cast<int>(20.f),static_cast<int>(30.f)},{13,13}));
    crosshairSprite.setOrigin({6.5f, 6.5f});
    crosshairSprite.setScale(sf::Vector2f(2.f,2.f));

    GameMap gameMap;

    constexpr float mapScale = 0.4f;
    if (!gameMap.load("assets/Levels/level1.txt",
                      "assets/Premium Content/Tileset with cell size 256x256.png", mapScale)) {
        std::cerr << "EROARE FATALA: Harta nu a putut fi incarcata." << std::endl;
        return -1;
    }
    sf::FloatRect mapBounds = gameMap.getPixelBounds();
    Player player(1640 * mapScale, 1360 * mapScale);
    ChaserEnemy::initAssets();

    std::vector<std::unique_ptr<EnemyBase>> enemies;
    const int MAX_ENEMIES = 10;
    const float RESPAWN_DELAY = 5.0f;
    sf::Clock respawnTimer;

    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        sf::Vector2f randomPos;
        do {
            float x = RandomGenerator::getFloat(mapBounds.position.x, mapBounds.position.x + mapBounds.size.x);
            float y = RandomGenerator::getFloat(mapBounds.position.y, mapBounds.position.y + mapBounds.size.y);
            randomPos = {x, y};
        } while (gameMap.isSolid(randomPos));

        enemies.push_back(std::make_unique<ChaserEnemy>());
        enemies.back()->setPosition(randomPos);
    }

    sf::Clock clock;
    sf::View camera;
    camera.setSize({1280, 720});
    camera.setCenter(player.getPosition());

    std::vector<Bullet> bullets;
    std::vector<std::unique_ptr<Effect>> effects;
    sf::Clock shootTimer;

    sf::Font ammoFont;
    if (!ammoFont.openFromFile("fonts/m6x11.ttf")) {
        std::cerr << "EROARE: Nu am putut incarca fontul fonts/m6x11.ttf" << std::endl;
        return -1;
    }
    sf::Text ammoText(ammoFont);
    ammoText.setFont(ammoFont);
    ammoText.setCharacterSize(48);
    ammoText.setFillColor(sf::Color::Black);

    sf::Clock playerDamageTimer;
    constexpr float PLAYER_IFRAME_DURATION = 0.3f;
    sf::RectangleShape debugHitbox;

    sf::Texture bloodEffectTexture;
    if (!bloodEffectTexture.loadFromFile("assets/enemies/impact_animation.png")) {
        std::cerr <<"EROARE la incarcarea imapct_aniamtion!!!";
        return -1;
    }
    std::vector<sf::IntRect> bloodEffectFrames;
    const int BLOOD_FRAME_WIDTH = 256;
    const int BLOOD_FRAME_HEIGHT = 256;
    const int BLOOD_COLS = 5;
    const int BLOOD_ROWS = 4;

    for (int y = 0;y < BLOOD_ROWS; ++y) {
        for (int x = 0;x < BLOOD_COLS; ++x) {
            bloodEffectFrames.push_back(sf::IntRect({x*BLOOD_FRAME_WIDTH,y*BLOOD_FRAME_HEIGHT},
                {BLOOD_FRAME_WIDTH,BLOOD_FRAME_HEIGHT}));
        }
    }

    while (window.isOpen()) {
        sf::Time dt = clock.restart();

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (event->is<sf::Event::KeyPressed>()) {
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::E) {
                    player.switchWeaponNext();
                }
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Q) {
                    player.switchWeaponPrev();
                }
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::R) {
                    player.reload();
                }
            }
        }

        sf::Vector2i mousePositionWindow = sf::Mouse::getPosition(window);
        sf::Vector2f mousePositionWorld = window.mapPixelToCoords(mousePositionWindow, camera);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && shootTimer.getElapsedTime().asSeconds() > player.
            getCurrentWeaponCooldown()
            && player.canShoot(mousePositionWorld)) {
            bullets.push_back((player.shoot(mousePositionWorld)));
            shootTimer.restart();
        }

        player.update(dt.asSeconds(), mousePositionWorld, gameMap);

        for (auto &bullet: bullets) {
            bullet.update(dt.asSeconds());
            if (!bullet.isImpacting())
            {
                for (auto& enemy : enemies)
                {
                    if (enemy->isDead()) continue;

                    if (enemy->getBounds().findIntersection(bullet.getBounds()))
                    {
                        float damage = bullet.getDamage();
                        enemy->takeDamage(damage);
                        bullet.hit();
                        float effectScale = 0.25f + (damage / 200.f);
                        effectScale = std::clamp(effectScale, 0.2f, 5.0f);
                        effects.push_back(std::make_unique<Effect>(
                            bloodEffectTexture,
                            bloodEffectFrames,
                            bullet.getPosition(),
                            0.01f,
                            sf::Vector2f(effectScale, effectScale)
                        ));

                        break;
                    }
                }
            }
        }

        if (respawnTimer.getElapsedTime().asSeconds() > RESPAWN_DELAY)
        {
            if (enemies.size() < MAX_ENEMIES)
            {
                int enemiesToSpawn = MAX_ENEMIES - static_cast<int>(enemies.size());

                for (int i = 0; i < enemiesToSpawn; ++i)
                {
                    sf::Vector2f randomPos;
                    do {
                        float x = RandomGenerator::getFloat(mapBounds.position.x, mapBounds.position.x + mapBounds.size.x);
                        float y = RandomGenerator::getFloat(mapBounds.position.y, mapBounds.position.y + mapBounds.size.y);
                        randomPos = {x, y};
                    } while (gameMap.isSolid(randomPos));

                    enemies.push_back(std::make_unique<ChaserEnemy>());
                    enemies.back()->setPosition(randomPos);
                }
            }
            respawnTimer.restart();
        }

        for (auto& enemy : enemies)
        {
            if (!enemy->isDead())
            {
                enemy->update(dt, player.getPosition(), gameMap);
            }
        }

        for (auto &effect : effects) {
            effect->update();
        }

        for (auto& enemy : enemies)
        {
            if (enemy->isDead()) continue;

            if (enemy->didAttackLand())
            {
                sf::FloatRect attackBox = enemy->getAttackHitbox();
                if (attackBox.findIntersection(player.getCollisionBounds()) &&
                    playerDamageTimer.getElapsedTime().asSeconds() > PLAYER_IFRAME_DURATION)
                {
                    sf::Vector2f knockbackDir = player.getPosition() - enemy->getPosition();
                    float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
                    if (length != 0.f) {
                        knockbackDir /= length;
                    } else {
                        knockbackDir = {1.f, 0.f};
                    }
                    player.takeDamage(10.f, knockbackDir);
                    playerDamageTimer.restart();

                    break;
                }
            }
        }

        sf::Vector2f playerPos = player.getPosition();
        sf::Vector2f viewSizeBlockedCamera = camera.getSize();


        sf::Vector2f viewCenter = playerPos;
        float halfViewX = viewSizeBlockedCamera.x / 2.0f;
        float halfViewY = viewSizeBlockedCamera.y / 2.0f;
        if (viewCenter.x < halfViewX) {
            viewCenter.x = halfViewX;
        }
        if (viewCenter.x > mapBounds.size.x - halfViewX) {
            viewCenter.x = mapBounds.size.x - halfViewX;
        }
        if (viewCenter.y < halfViewY) {
            viewCenter.y = halfViewY;
        }
        if (viewCenter.y > mapBounds.size.y - halfViewY) {
            viewCenter.y = mapBounds.size.y - halfViewY;
        }

        camera.setCenter(viewCenter);

        std::string ammoString = std::to_string(player.getCurrentAmmo()) + " / " + std::to_string(
                                     player.getReserveAmmo());
        std::string weaponNameString = player.getCurrentWeaponName();
        std::string uiText = weaponNameString + '\n';
        std::string ammoFinal = uiText + ammoString;
        ammoText.setString(ammoFinal);

        window.clear(sf::Color(30, 30, 30));
        window.setView(camera);
        window.draw(gameMap);
        player.drawWorld(window);

        for (auto& enemy : enemies)
        {
            enemy->draw(window);
        }

        for (const auto& effect : effects) {
            effect->draw(window);
        }

        for (auto &bullet: bullets) {
            bullet.draw(window);

            if (!bullet.isImpacting()) {
                sf::Vector2f pos = bullet.getPosition();
                sf::Vector2f vel = bullet.getVelocity();

                float length = std::sqrt(vel.x * vel.x + vel.y * vel.y);
                sf::Vector2f normVel(0.f, 0.f);
                if (length != 0.f) {
                    normVel = vel / length;
                }
                float checkOffset = 25.f;

                sf::Vector2f checkPosition = pos + normVel * checkOffset;
                if (gameMap.isSolid(checkPosition)) {
                    bullet.hit();
                }
            }
        }

        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                           [](const Bullet &bullet) {
                               return bullet.isDead();
                           }
            ),
            bullets.end()
        );
        std::erase_if(effects, [](const auto& effect) {
            return effect->isDead();
        });
        std::erase_if(enemies, [](const auto& enemy) {
            return enemy->isDead();
    });

        window.setView(window.getDefaultView());

        crosshairSprite.setPosition(static_cast<sf::Vector2f>(mousePositionWindow));
        ammoText.setOrigin({0.f, 0.f});

        sf::Vector2f viewSize = static_cast<sf::Vector2f>(window.getSize());

        auto charSize = static_cast<float>(ammoText.getCharacterSize());
        float numLines = 2.f;
        float totalTextHeight = (charSize * numLines) * 1.1f;
        float positionY = viewSize.y - totalTextHeight - 10.f;
        ammoText.setPosition({10.f, positionY});

        player.drawUI(window);
        window.draw(ammoText);
        window.draw(crosshairSprite);

        window.display();
    }

    return 0;
}