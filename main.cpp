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
    if (!gameMap.load("../assets/Levels/level1.json",
                      "../assets/Premium Content/Tileset with cell size 256x256.png")) {
        std::cerr << "EROARE FATALA: Harta nu a putut fi incarcata." << std::endl;
        return -1;
                      }
    constexpr float mapScale = 0.4f;
    Player player(1000.f * mapScale, 1000.f * mapScale);

    Enemy enemy1(100.f, 200.f);
    std::vector<Enemy> enemies;
    enemies.push_back(enemy1);

    player.takeDamage(25.f);
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
    if (!ammoFont.openFromFile("../fonts/m6x11.ttf")) {
        std::cerr << "EROARE: Nu am putut incarca fontul assets/arial.ttf" << std::endl;
        return -1;
    }
    sf::Text ammoText(ammoFont);
    ammoText.setFont(ammoFont);
    ammoText.setCharacterSize(56);
    ammoText.setFillColor(sf::Color::Black);

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
        sf::Vector2f mousePositionWorld = window.mapPixelToCoords(mousePositionWindow,camera);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && shootTimer.getElapsedTime().asSeconds() > player.getCurrentWeaponCooldown()
            && player.canShoot(mousePositionWorld)) {
            //Creez glnt si trag
            bullets.push_back((player.shoot(mousePositionWorld)));
            shootTimer.restart();
        }


        player.update(dt.asSeconds(), mousePositionWorld);
        camera.setCenter(player.getPosition());

        sf::Vector2f playerPos = player.getPosition();
        for ([[maybe_unused]] auto& enemy : enemies) {
            Enemy::update(dt.asSeconds(), playerPos);
        }

        for (auto& bullet : bullets) {
            bullet.update(dt.asSeconds());
        }

        std::string ammoString = std::to_string(player.getCurrentAmmo())+ " / " + std::to_string(player.getReserveAmmo());
        ammoText.setString(ammoString);

        window.clear(sf::Color(30, 30, 30));
        window.setView(camera);
        gameMap.draw(window);
        player.draw(window);

        for (auto& enemy : enemies) {
            enemy.draw(window);
        }
        for (auto& bullet : bullets) {
            bullet.draw(window);
        }
        window.setView(window.getDefaultView());

        sf::Vector2f viewSize = window.getDefaultView().getSize();
        ammoText.setPosition({10.f, viewSize.y - static_cast<float>(ammoText.getCharacterSize()) - 10.f});

        window.draw(ammoText);
        player.drawUI(window);

        window.display();
    }

    return 0;
}