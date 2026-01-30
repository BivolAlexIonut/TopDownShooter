#include "Game.h"
#include "GameExceptions.h"
#include "RandomGenerator.h"
#include "ChaserEnemy.h"
#include "GhostEnemy.h"
#include "DevilEnemy.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Game::Game()
    : m_window(sf::VideoMode({1280, 720}), "Top-Down Shooter")
    , m_state(GameState::MainMenu)
{
    m_window.setFramerateLimit(60);
    m_window.setMouseCursorVisible(false);
    
    m_camera.setSize({1280, 720});
    m_uiView.setSize(static_cast<sf::Vector2f>(m_window.getSize()));
    m_uiView.setCenter(static_cast<sf::Vector2f>(m_window.getSize()) / 2.f);

    ResourceManager::getInstance().loadAll();

    m_crosshairSprite.emplace(ResourceManager::getInstance().getTexture("crosshair"));
    m_crosshairSprite->setTextureRect(sf::IntRect({20, 30}, {13, 13}));
    m_crosshairSprite->setOrigin({6.5f, 6.5f});
    m_crosshairSprite->setScale({2.f, 2.f});

    m_coinIconSprite.emplace(ResourceManager::getInstance().getTexture("coin"));
    m_coinIconSprite->setTextureRect(sf::IntRect({0, 0}, {20, 20}));
    m_coinIconSprite->setScale({1.5f, 1.5f});

    m_ammoText.emplace(ResourceManager::getInstance().getFont());
    m_ammoText->setCharacterSize(48);
    m_ammoText->setFillColor(sf::Color::Black);

    m_coinText.emplace(ResourceManager::getInstance().getFont());
    m_coinText->setCharacterSize(48);
    m_coinText->setFillColor(sf::Color(240, 180, 0));

    m_menuTitle.emplace(ResourceManager::getInstance().getFont());
    m_menuTitle->setString("TOP-DOWN SHOOTER");
    m_menuTitle->setCharacterSize(72);
    m_menuTitle->setFillColor(sf::Color::White);

    m_menuStart.emplace(ResourceManager::getInstance().getFont());
    m_menuStart->setString("Apasati ENTER pentru a incepe");
    m_menuStart->setCharacterSize(48);
    m_menuStart->setFillColor(sf::Color::Yellow);

    m_menuExit.emplace(ResourceManager::getInstance().getFont());
    m_menuExit->setString("Apasati ESCAPE pentru a iesi");
    m_menuExit->setCharacterSize(48);
    m_menuExit->setFillColor(sf::Color::White);

    m_gameOverTitle.emplace(ResourceManager::getInstance().getFont());
    m_gameOverTitle->setString("GAME OVER");
    m_gameOverTitle->setCharacterSize(72);
    m_gameOverTitle->setFillColor(sf::Color::Red);

    m_gameOverRestart.emplace(ResourceManager::getInstance().getFont());
    m_gameOverRestart->setString("Apasati R pentru a reveni la meniu");
    m_gameOverRestart->setCharacterSize(48);
    m_gameOverRestart->setFillColor(sf::Color::Yellow);

    const std::string devilBasePath = "assets/enemies/Flying Demon 2D Pixel Art/Sprites/with_outline/";
    const std::string devilProjectilePath = "assets/enemies/Flying Demon 2D Pixel Art/Sprites/projectile.png";
    ChaserEnemy::initAssets();
    DevilEnemy::initAssets(devilBasePath);
    DevilProjectile::initAssets(devilProjectilePath);
    GhostEnemy::initAssets();
    Coin::initAssets();
    m_upgradeMenu.loadFont("fonts/m6x11.ttf");

    for (int i = 0; i < 20; ++i) {
        m_ghostImpactFrames.push_back(sf::IntRect({(i % 5) * 256, (i / 5) * 256}, {256, 256}));
    }
    for (int i = 0; i < 20; ++i) {
        m_bloodEffectFrames.push_back(sf::IntRect({(i % 5) * 256, (i / 5) * 256}, {256, 256}));
    }
}

void Game::restartGame() {
    constexpr float mapScale = 0.4f;
    if (!m_gameMap.load("assets/Levels/level1.txt", "assets/Premium Content/Tileset with cell size 256x256.png", mapScale)) {
        throw MapLoadException("level1.txt", "Eroare la incarcare");
    }
    m_mapBounds = m_gameMap.getPixelBounds();
    m_player = std::make_unique<Player>(1640 * mapScale, 1360 * mapScale, ResourceManager::getInstance().getSoundBuffers());
    
    m_enemyManager.reset();
    m_projectileManager.clear();
    m_effectManager.clear();
    m_pickableManager.clear();

    m_clock.restart();
    m_shootTimer.restart();
    m_playerDamageTimer.restart();

    if (ResourceManager::getInstance().getBackgroundMusic().getStatus() != sf::SoundStream::Status::Playing)
        ResourceManager::getInstance().getBackgroundMusic().play();

    m_state = GameState::Playing;
}

void Game::run() {
    while (m_window.isOpen()) {
        sf::Time dt = m_clock.restart();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    while (auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) m_window.close();
        if (event->is<sf::Event::Resized>()) {
            auto res = event->getIf<sf::Event::Resized>();
            m_camera.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f})); 
            m_uiView.setSize(static_cast<sf::Vector2f>(res->size));
            m_uiView.setCenter(static_cast<sf::Vector2f>(res->size) / 2.f);
        }
        if (event->is<sf::Event::KeyPressed>()) {
            auto key = event->getIf<sf::Event::KeyPressed>()->code;
            if (m_state == GameState::MainMenu && key == sf::Keyboard::Key::Enter) restartGame();
            else if (m_state == GameState::GameOver && key == sf::Keyboard::Key::R) m_state = GameState::MainMenu;
            else if (key == sf::Keyboard::Key::Escape) m_window.close();
            else if (key == sf::Keyboard::Key::Tab) {
                m_state = (m_state == GameState::Playing) ? GameState::Paused : GameState::Playing;
                if (m_state == GameState::Paused) m_upgradeMenu.setDisplayedWeapon(m_player->getCurrentWeaponIndex());
            }
            
            if (m_state == GameState::Paused) m_upgradeMenu.handleInput(key, *m_player);
            else if (m_state == GameState::Playing) {
                if (key == sf::Keyboard::Key::E) m_player->switchWeaponNext();
                if (key == sf::Keyboard::Key::Q) m_player->switchWeaponPrev();
                if (key == sf::Keyboard::Key::R) {
                    auto reload = m_player->reload();
                    if (!reload.first.empty()) {
                        m_activeSounds.emplace_back(ResourceManager::getInstance().getSoundBuffer(reload.first));
                        m_activeSounds.back().setPitch(ResourceManager::getInstance().getSoundBuffer(reload.first).getDuration().asSeconds() / reload.second);
                        m_activeSounds.back().play();
                    }
                }
            }
        }
    }
}

void Game::update(sf::Time dt) {
    if (m_state == GameState::Playing) {
        sf::Vector2f mouseWorld = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window), m_camera);
        
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && 
            m_shootTimer.getElapsedTime().asSeconds() > m_player->getCurrentWeaponCooldown() && 
            m_player->canShoot(mouseWorld)) {
            m_projectileManager.addBullet(m_player->shoot(mouseWorld));
            m_activeSounds.emplace_back(ResourceManager::getInstance().getSoundBuffer(m_player->getShootSoundKey()));
            m_activeSounds.back().setPitch(RandomGenerator::get<float>(0.95f, 1.05f));
            m_activeSounds.back().play();
            m_shootTimer.restart();
        }

        m_player->update(dt.asSeconds(), mouseWorld, m_gameMap);
        m_projectileManager.update(dt.asSeconds(), m_gameMap, *m_player, m_playerDamageTimer, m_playerIframeDuration, m_activeSounds, ResourceManager::getInstance().getSoundBuffer("player_hurt"));
        
        std::vector<std::unique_ptr<Coin>> newCoins;
        std::vector<std::unique_ptr<DevilProjectile>> newProjectiles;
        m_enemyManager.update(dt.asSeconds(), *m_player, m_gameMap, m_activeSounds, newCoins, newProjectiles, m_mapBounds, m_playerDamageTimer, m_playerIframeDuration);
        
        for (auto& coin : newCoins) m_pickableManager.addCoin(std::move(coin));
        for (auto& proj : newProjectiles) m_projectileManager.addEnemyProjectile(std::move(proj));

        m_enemyManager.handleBulletCollisions(m_projectileManager.getBullets(), m_activeSounds, 
            m_effectManager,
            ResourceManager::getInstance().getTexture("ghost_impact"), m_ghostImpactFrames, 
            ResourceManager::getInstance().getTexture("blood_effect"), m_bloodEffectFrames);

        m_pickableManager.update(dt.asSeconds(), *m_player, m_activeSounds, ResourceManager::getInstance().getSoundBuffer("coin_pickup"));
        m_effectManager.update();

        sf::Vector2f center = m_player->getPosition();
        sf::Vector2f size = m_camera.getSize();
        center.x = std::clamp(center.x, size.x/2.f, m_mapBounds.size.x - size.x/2.f);
        center.y = std::clamp(center.y, size.y/2.f, m_mapBounds.size.y - size.y/2.f);
        m_camera.setCenter(center);

        if (m_player->isDead()) { m_state = GameState::GameOver; ResourceManager::getInstance().getBackgroundMusic().stop(); }
    } else if (m_state == GameState::Paused) m_upgradeMenu.update(*m_player);

    std::erase_if(m_activeSounds, [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Status::Stopped; });
}

void Game::render() {
    m_window.clear(sf::Color(30, 30, 30));
    if (m_state == GameState::Playing || m_state == GameState::Paused) {
        m_window.setView(m_camera);
        m_window.draw(m_gameMap);
        m_gameMap.updateAndDrawCooldowns(m_window);
        m_pickableManager.draw(m_window);
        m_player->drawWorld(m_window);
        m_enemyManager.draw(m_window);
        m_effectManager.draw(m_window);
        m_projectileManager.draw(m_window);

        m_window.setView(m_uiView);
        m_ammoText->setString(m_player->getCurrentWeaponName() + "\n" + std::to_string(m_player->getCurrentAmmo()) + " / " + std::to_string(m_player->getReserveAmmo()));
        float textH = static_cast<float>(m_ammoText->getCharacterSize()) * 2.2f;
        m_ammoText->setPosition({10.f, m_uiView.getSize().y - textH - 10.f});
        m_coinText->setString(std::to_string(m_player->getCoinCount()));
        m_coinIconSprite->setPosition({m_uiView.getSize().x - 140.f, 15.f});
        m_coinText->setPosition({m_uiView.getSize().x - m_coinText->getGlobalBounds().size.x - 20.f, 10.f});

        m_player->drawUI(m_window);
        m_window.draw(*m_ammoText); m_window.draw(*m_coinText); m_window.draw(*m_coinIconSprite);
        if (m_state == GameState::Playing) {
            m_crosshairSprite->setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window)));
            m_window.draw(*m_crosshairSprite);
        }
        if (m_state == GameState::Paused) m_upgradeMenu.draw(m_window, m_uiView);
    } else {
        m_window.setView(m_uiView);
        sf::Vector2f sz = m_uiView.getSize();
        if (m_state == GameState::MainMenu) {
            m_menuTitle->setPosition({sz.x/2.f - m_menuTitle->getGlobalBounds().size.x/2.f, 200.f});
            m_menuStart->setPosition({sz.x/2.f - m_menuStart->getGlobalBounds().size.x/2.f, 350.f});
            m_menuExit->setPosition({sz.x/2.f - m_menuExit->getGlobalBounds().size.x/2.f, 420.f});
            m_window.draw(*m_menuTitle); m_window.draw(*m_menuStart); m_window.draw(*m_menuExit);
        } else {
            m_gameOverTitle->setPosition({sz.x/2.f - m_gameOverTitle->getGlobalBounds().size.x/2.f, 200.f});
            m_gameOverRestart->setPosition({sz.x/2.f - m_gameOverRestart->getGlobalBounds().size.x/2.f, 350.f});
            m_window.draw(*m_gameOverTitle); m_window.draw(*m_gameOverRestart);
        }
    }
    m_window.display();
}