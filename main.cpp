#include <SFML/Graphics.hpp>
#include "Player.h"
#include "GameMap.h"
#include "Bullet.h"
#include <iostream>
#include <vector>
#include "Enemy.h"
#include <ostream>


int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Top-Down Shooter");
    window.setFramerateLimit(60);

    GameMap gameMap;

    constexpr float mapScale = 0.4f;
    if (!gameMap.load("assets/Levels/level1.txt",
                      "assets/Premium Content/Tileset with cell size 256x256.png",mapScale)) {
        std::cerr << "EROARE FATALA: Harta nu a putut fi incarcata." << std::endl;
        return -1;
                      }
    Player player(1640 * mapScale, 1360 * mapScale);

    Enemy enemy1(100.f, 200.f);
    std::vector<Enemy> enemies;
    enemies.push_back(enemy1);

    player.takeDamage(25.f);//Testez Ui pentru healthbar
    enemies[0].takeDamage(10.f);

    sf::Clock clock;
    sf::View camera;
    camera.setSize({1280,720});
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
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Backspace) {
                    window.close();
                }
            }
        }

        sf::Vector2i mousePositionWindow = sf::Mouse::getPosition(window);
        sf::Vector2f mousePositionWorld = window.mapPixelToCoords(mousePositionWindow,camera);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && shootTimer.getElapsedTime().asSeconds() > player.getCurrentWeaponCooldown()
            && player.canShoot(mousePositionWorld)) {
            bullets.push_back((player.shoot(mousePositionWorld)));
            shootTimer.restart();
        }

        player.update(dt.asSeconds(), mousePositionWorld,gameMap);
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

        for (auto& bullet : bullets) {
            bullet.update(dt.asSeconds());
        }

        std::string ammoString = std::to_string(player.getCurrentAmmo())+ " / " + std::to_string(player.getReserveAmmo());
        std::string weaponNameString = player.getCurrentWeaponName();
        std::string uiText = weaponNameString + '\n';
        std::string ammoFinal = uiText+ammoString;
        ammoText.setString(ammoFinal);

        window.clear(sf::Color(30, 30, 30));
        window.setView(camera);
        window.draw(gameMap);
        player.drawWorld(window);

        for (auto& bullet : bullets) {
            bullet.draw(window);
        }
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