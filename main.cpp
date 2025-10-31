#include <cmath>
#include <iostream>
#include <ostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "ChaserEnemy.h"
#include "Enemy.h"
#include "GameMap.h"
#include "Player.h"


int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Top-Down Shooter");
    window.setFramerateLimit(60);

    GameMap gameMap;

    constexpr float mapScale = 0.4f;
    if (!gameMap.load("assets/Levels/level1.txt",
                      "assets/Premium Content/Tileset with cell size 256x256.png", mapScale)) {
        std::cerr << "EROARE FATALA: Harta nu a putut fi incarcata." << std::endl;
        return -1;
    }
    Player player(1640 * mapScale, 1360 * mapScale);

    ChaserEnemy chaser1;
    chaser1.setPosition({player.getPosition().x + 200.f, player.getPosition().y});


    Enemy enemy1(100.f, 200.f);
    std::vector<Enemy> enemies;
    enemies.push_back(enemy1);

    enemies[0].takeDamage(10.f);

    sf::Clock clock;
    sf::View camera;
    camera.setSize({1280, 720});
    camera.setCenter(player.getPosition());

    //Bullets
    std::vector<Bullet> bullets;
    sf::Clock shootTimer;

    //UI Ammo
    sf::Font ammoFont;
    if (!ammoFont.openFromFile("fonts/m6x11.ttf")) {
        std::cerr << "EROARE: Nu am putut incarca fontul fonts/m6x11.ttf" << std::endl;
        return -1;
    }
    sf::Text ammoText(ammoFont);
    ammoText.setFont(ammoFont);
    ammoText.setCharacterSize(48);
    ammoText.setFillColor(sf::Color::Black);

    //Timer pentru damage
    sf::Clock playerDamageTimer;
    const float PLAYER_IFRAME_DURATION = 0.3f;//am o secunda invincibilitate
    sf::RectangleShape debugHitbox;
    debugHitbox.setFillColor(sf::Color::Transparent); // Fără umplere
    debugHitbox.setOutlineThickness(2.f);

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
        chaser1.update(dt,player.getPosition());

        //atacul inamicului
        if (chaser1.didAttackLand())
        {
            sf::FloatRect attackBox = chaser1.getAttackHitbox();
            if (attackBox.findIntersection(player.getCollisionBounds()) &&
                playerDamageTimer.getElapsedTime().asSeconds() > PLAYER_IFRAME_DURATION)
            {
                sf::Vector2f knockbackDir = player.getPosition() - chaser1.getPosition();
                float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
                if (length != 0.f) {
                    knockbackDir /= length;
                } else {
                    knockbackDir = {1.f, 0.f};
                }
                player.takeDamage(10.f, knockbackDir);
                playerDamageTimer.restart();
            }
        }

        //Obtin pozitiile si dimensiunile
        sf::Vector2f playerPos = player.getPosition();
        sf::Vector2f viewSizeBlockedCamera = camera.getSize();
        sf::FloatRect mapBounds = gameMap.getPixelBounds();

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

        for (auto &bullet: bullets) {
            bullet.update(dt.asSeconds());
        }

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
        chaser1.draw(window);


        //DEBUG PENTRU HITBOX
        /*
        sf::FloatRect playerBounds = player.getCollisionBounds();
        debugHitbox.setPosition(playerBounds.position);
        debugHitbox.setSize(playerBounds.size);
        debugHitbox.setOutlineColor(sf::Color::Blue);
        window.draw(debugHitbox);

        sf::FloatRect attackBox = chaser1.getAttackHitbox();
        debugHitbox.setPosition(attackBox.position);
        debugHitbox.setSize(attackBox.size);

        if (chaser1.isAttacking()) {
            if (chaser1.didAttackLand()) {
                debugHitbox.setOutlineColor(sf::Color::Red);
            } else {
                debugHitbox.setOutlineColor(sf::Color::Green);
            }
            window.draw(debugHitbox);
        }
        */

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

        window.setView(window.getDefaultView());

        ammoText.setOrigin({0.f, 0.f});

        sf::Vector2f viewSize = static_cast<sf::Vector2f>(window.getSize());

        auto charSize = static_cast<float>(ammoText.getCharacterSize());
        float numLines = 2.f;
        float totalTextHeight = (charSize * numLines) * 1.1f;
        float positionY = viewSize.y - totalTextHeight - 10.f;
        ammoText.setPosition({10.f, positionY});

        player.drawUI(window);
        window.draw(ammoText);

        window.display();
    }

    return 0;
}

