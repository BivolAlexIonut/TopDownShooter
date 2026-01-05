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
    , m_crosshairSprite(m_crosshairTexture)
    , m_coinIconSprite(m_coinIconTexture)
    , m_ammoText(m_ammoFont)
    , m_coinText(m_ammoFont)
    , m_menuTitle(m_ammoFont)
    , m_menuStart(m_ammoFont)
    , m_menuExit(m_ammoFont)
    , m_gameOverTitle(m_ammoFont)
    , m_gameOverRestart(m_ammoFont)
    , m_enemyManager(m_soundBuffers)
    , m_mapBounds({0.f, 0.f}, {0.f, 0.f})
{
    m_window.setFramerateLimit(60);
    m_window.setMouseCursorVisible(false);
    
    m_camera.setSize({1280, 720});
    m_uiView.setSize(static_cast<sf::Vector2f>(m_window.getSize()));
    m_uiView.setCenter(static_cast<sf::Vector2f>(m_window.getSize()) / 2.f);

    loadAssets();

    // Configurare UI
    // Textura este deja setata in lista de initializare, trebuie doar sa setam proprietatile
    m_crosshairSprite.setTextureRect(sf::IntRect({static_cast<int>(20.f), static_cast<int>(30.f)}, {13, 13}));
    m_crosshairSprite.setOrigin({6.5f, 6.5f});
    m_crosshairSprite.setScale(sf::Vector2f(2.f, 2.f));

    m_coinIconSprite.setTextureRect(sf::IntRect({0, 0}, {20, 20}));
    m_coinIconSprite.setScale({1.5f, 1.5f});

    m_ammoText.setCharacterSize(48);
    m_ammoText.setFillColor(sf::Color::Black);

    m_coinText.setCharacterSize(48);
    m_coinText.setFillColor(sf::Color(240, 180, 0));

    // Configurare Meniu
    m_menuTitle.setString("TOP-DOWN SHOOTER");
    m_menuTitle.setCharacterSize(72);
    m_menuTitle.setFillColor(sf::Color::White);

    m_menuStart.setString("Apasati ENTER pentru a incepe");
    m_menuStart.setCharacterSize(48);
    m_menuStart.setFillColor(sf::Color::Yellow);

    m_menuExit.setString("Apasati ESCAPE pentru a iesi");
    m_menuExit.setCharacterSize(48);
    m_menuExit.setFillColor(sf::Color::White);

    m_gameOverTitle.setString("GAME OVER");
    m_gameOverTitle.setCharacterSize(72);
    m_gameOverTitle.setFillColor(sf::Color::Red);

    m_gameOverRestart.setString("Apasati R pentru a reveni la meniu");
    m_gameOverRestart.setCharacterSize(48);
    m_gameOverRestart.setFillColor(sf::Color::Yellow);

    // Configurare frame-uri pentru efecte
    const int GHOST_IMPACT_W = 256;
    const int GHOST_IMPACT_H = 256;
    const int GHOST_IMPACT_COLS = 5;
    const int GHOST_IMPACT_TOTAL_FRAMES = 20;

    for (int i = 0; i < GHOST_IMPACT_TOTAL_FRAMES; ++i) {
        int row = i / GHOST_IMPACT_COLS;
        int col = i % GHOST_IMPACT_COLS;
        m_ghostImpactFrames.push_back(sf::IntRect(
            {col * GHOST_IMPACT_W, row * GHOST_IMPACT_H},
            {GHOST_IMPACT_W, GHOST_IMPACT_H}
        ));
    }

    const int BLOOD_FRAME_WIDTH = 256;
    const int BLOOD_FRAME_HEIGHT = 256;
    const int BLOOD_COLS = 5;
    const int BLOOD_ROWS = 4;

    for (int y = 0; y < BLOOD_ROWS; ++y) {
        for (int x = 0; x < BLOOD_COLS; ++x) {
            m_bloodEffectFrames.push_back(sf::IntRect({x * BLOOD_FRAME_WIDTH, y * BLOOD_FRAME_HEIGHT},
                                                    {BLOOD_FRAME_WIDTH, BLOOD_FRAME_HEIGHT}));
        }
    }
}

void Game::loadAssets() {
    if (!m_crosshairTexture.loadFromFile("assets/ui/crosshair.png")) {
        std::cerr << "NU AM PUTUT INCARCA ui/crosshair.png" << std::endl;
    }
    
    if (!m_coinIconTexture.loadFromFile("assets/coin.png")) {
        throw AssetLoadException("coin.png");
    }

    if (!m_ghostImpactTexture.loadFromFile("assets/enemies/death_animation-ghost.png")) {
        throw AssetLoadException("death_animation-ghost.png");
    }

    if (!m_bloodEffectTexture.loadFromFile("assets/enemies/death_animation.png")) {
        throw AssetLoadException("death_animation.png");
    }

    constexpr float mapScale = 0.4f;
    if (!m_gameMap.load("assets/Levels/level1.txt",
                      "assets/Premium Content/Tileset with cell size 256x256.png", mapScale)) {
        throw MapLoadException("level1.txt", "Functia load a returnat false.");
    }
    m_mapBounds = m_gameMap.getPixelBounds();

    const std::string devilBasePath = "assets/enemies/Flying Demon 2D Pixel Art/Sprites/with_outline/";
    const std::string devilProjectilePath = "assets/enemies/Flying Demon 2D Pixel Art/Sprites/projectile.png";

    ChaserEnemy::initAssets();
    DevilEnemy::initAssets(devilBasePath);
    DevilProjectile::initAssets(devilProjectilePath);
    GhostEnemy::initAssets();
    Coin::initAssets();

    if (!m_ammoFont.openFromFile("fonts/m6x11.ttf")) {
        throw FontLoadException("fonts/m6x11.ttf");
    }
    if (!m_upgradeMenu.loadFont("fonts/m6x11.ttf")) {
        throw FontLoadException("m6x11.ttf pentru meniu");
    }

    std::cout << "Se incarca sunetele..." << std::endl;
    // Incarcare toate sunetele
    auto loadSound = [&](const std::string& key, const std::string& path) {
        if (!m_soundBuffers[key].loadFromFile(path)) {
            throw AssetLoadException(path);
        }
    };

    loadSound("player_step", "assets/sounds/steps/Steps_floor-001.wav");
    loadSound("pistol_shoot", "assets/sounds/shoot/pistol.wav");
    loadSound("chaser_hurt", "assets/sounds/enemies/chaser/Orc_Damage.wav");
    loadSound("ghost_hurt", "assets/sounds/enemies/ghost/Ghost_Death.wav");
    loadSound("devil_hurt", "assets/sounds/enemies/devil/Behemoth_Death.wav");
    loadSound("chaser_attack", "assets/sounds/enemies/chaser/Attack.wav");
    loadSound("ghost_attack", "assets/sounds/enemies/ghost/Attack.wav");
    loadSound("devil_attack", "assets/sounds/enemies/devil/Attack.wav");
    loadSound("chaser_move", "assets/sounds/enemies/chaser/Move.wav");
    loadSound("ghost_move", "assets/sounds/enemies/chaser/Move.wav");
    loadSound("devil_move", "assets/sounds/enemies/chaser/Move.wav");
    
    loadSound("pistol_reload", "assets/sounds/reload/pistol_reload.wav");
    loadSound("tommygun_shoot", "assets/sounds/shoot/tommygun.wav");
    loadSound("tommygun_reload", "assets/sounds/reload/tommygun_reload.wav");
    loadSound("rpg_shoot", "assets/sounds/shoot/rpg.wav");
    loadSound("rpg_reload", "assets/sounds/reload/rpg_reload.wav");
    loadSound("smg_shoot", "assets/sounds/shoot/smg.wav");
    loadSound("smg_reload", "assets/sounds/reload/smg_reload.wav");
    loadSound("shotgun_shoot", "assets/sounds/shoot/shotgun.wav");
    loadSound("shotgun_reload", "assets/sounds/reload/shotgun_reload.wav");
    loadSound("sniper_shoot", "assets/sounds/shoot/sniper.wav");
    loadSound("sniper_reload", "assets/sounds/reload/sniper_reload.wav");
    loadSound("coin_drop", "assets/sounds/coin_drop.wav");
    loadSound("coin_pickup", "assets/sounds/coin_pickup.wav");
    loadSound("player_hurt", "assets/sounds/player/damage.wav");

    if (!m_backgroundMusic.openFromFile("assets/sounds/background/retro_metal.ogg")) {
        throw AssetLoadException("retro_metal.ogg");
    }
    m_backgroundMusic.setVolume(60.f);
    m_backgroundMusic.setLooping(true);
}

void Game::restartGame() {
    constexpr float mapScale = 0.4f;
    m_player = std::make_unique<Player>(1640 * mapScale, 1360 * mapScale, m_soundBuffers);
    
    m_enemyManager.reset();
    m_bullets.clear();
    m_effects.clear();
    m_coins.clear();
    m_enemyProjectiles.clear();

    m_clock.restart();
    m_shootTimer.restart();
    m_playerDamageTimer.restart();

    if (m_backgroundMusic.getStatus() != sf::SoundStream::Status::Playing)
        m_backgroundMusic.play();

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
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        if (event->is<sf::Event::Resized>()) {
            auto resized = event->getIf<sf::Event::Resized>();
            auto newWidth = static_cast<float>(resized->size.x);
            auto newHeight = static_cast<float>(resized->size.y);

            float targetRatio = 1280.f / 720.f;
            float windowRatio = newWidth / newHeight;

            float viewportX = 0.f;
            float viewportY = 0.f;
            float viewportWidth = 1.f;
            float viewportHeight = 1.f;

            if (windowRatio > targetRatio) {
                viewportWidth = targetRatio / windowRatio;
                viewportX = (1.f - viewportWidth) / 2.f;
            } else if (windowRatio < targetRatio) {
                viewportHeight = windowRatio / targetRatio;
                viewportY = (1.f - viewportHeight) / 2.f;
            }

            m_camera.setViewport(sf::FloatRect({viewportX, viewportY}, {viewportWidth, viewportHeight}));
            m_uiView.setSize({newWidth, newHeight});
            m_uiView.setCenter({newWidth / 2.f, newHeight / 2.f});
        }

        if (event->is<sf::Event::KeyPressed>()) {
            auto keyEvent = event->getIf<sf::Event::KeyPressed>();
            
            if (m_state == GameState::MainMenu) {
                if (keyEvent->code == sf::Keyboard::Key::Enter) {
                    restartGame();
                }
                if (keyEvent->code == sf::Keyboard::Key::Escape) {
                    m_window.close();
                }
            }
            else if (m_state == GameState::GameOver) {
                if (keyEvent->code == sf::Keyboard::Key::R) {
                    m_state = GameState::MainMenu;
                }
                if (keyEvent->code == sf::Keyboard::Key::Escape) {
                    m_window.close();
                }
            }
            else {
                if (keyEvent->code == sf::Keyboard::Key::Tab) {
                    if (m_state == GameState::Playing) {
                        m_state = GameState::Paused;
                        m_upgradeMenu.setDisplayedWeapon(m_player->getCurrentWeaponIndex());
                    } else {
                        m_state = GameState::Playing;
                    }
                }
                if (m_state == GameState::Paused) {
                    if (keyEvent->code == sf::Keyboard::Key::Backspace) {
                        m_window.close();
                    }
                    m_upgradeMenu.handleInput(keyEvent->code, *m_player);
                }
                else if (m_state == GameState::Playing) {
                    if (keyEvent->code == sf::Keyboard::Key::E) {
                        m_player->switchWeaponNext();
                    }
                    if (keyEvent->code == sf::Keyboard::Key::Q) {
                        m_player->switchWeaponPrev();
                    }
                    if (keyEvent->code == sf::Keyboard::Key::R) {
                        std::pair<std::string, float> reloadData = m_player->reload();
                        std::string reloadSoundKey = reloadData.first;
                        float reloadAnimDuration = reloadData.second;

                        if (!reloadSoundKey.empty()) {
                            m_activeSounds.emplace_back(m_soundBuffers[reloadSoundKey]);
                            sf::Sound &newSound = m_activeSounds.back();
                            float soundOriginalDuration = m_soundBuffers[reloadSoundKey].getDuration().asSeconds();
                            float newPitch = 1.0f;
                            if (reloadAnimDuration > 0.01f && soundOriginalDuration > 0.01f) {
                                newPitch = soundOriginalDuration / reloadAnimDuration;
                            }
                            newSound.setPitch(newPitch);
                            newSound.play();
                        }
                    }
                    if (keyEvent->code == sf::Keyboard::Key::Backspace) {
                        m_window.close();
                    }
                }
            }
        }
    }
}

void Game::update(sf::Time dt) {
    if (m_state == GameState::Playing) {
        sf::Vector2i mousePositionWindow = sf::Mouse::getPosition(m_window);
        sf::Vector2f mousePositionWorld = m_window.mapPixelToCoords(mousePositionWindow, m_camera);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && 
            m_shootTimer.getElapsedTime().asSeconds() > m_player->getCurrentWeaponCooldown() && 
            m_player->canShoot(mousePositionWorld)) {
            
            m_bullets.push_back(m_player->shoot(mousePositionWorld));
            std::string shootSoundKey = m_player->getShootSoundKey();
            if (!shootSoundKey.empty()) {
                m_activeSounds.emplace_back(m_soundBuffers[shootSoundKey]);
                m_activeSounds.back().setPitch(RandomGenerator::getFloat(0.95f, 1.05f));
                m_activeSounds.back().play();
            }
            m_shootTimer.restart();
        }

        m_player->update(dt.asSeconds(), mousePositionWorld, m_gameMap);

        for (auto &bullet: m_bullets) {
            bullet.update(dt.asSeconds());
        }

        //coliziuni gloante-inamici catre EnemyManager
        m_enemyManager.handleBulletCollisions(m_bullets, m_activeSounds, m_effects, 
                                              m_ghostImpactTexture, m_ghostImpactFrames, 
                                              m_bloodEffectTexture, m_bloodEffectFrames);
        
        // Coliziuni gloante cu peretii
        for (auto &bullet: m_bullets) {
            if (!bullet.isImpacting()) {
                sf::Vector2f pos = bullet.getPosition();
                sf::Vector2f vel = bullet.getVelocity();
                float length = std::sqrt(vel.x * vel.x + vel.y * vel.y);
                sf::Vector2f normVel(0.f, 0.f);
                if (length != 0.f) normVel = vel / length;
                
                if (m_gameMap.isSolid(pos + normVel * 25.f)) {
                    bullet.hit();
                }
            }
        }

        m_enemyManager.update(dt.asSeconds(), *m_player, m_gameMap, 
                              m_activeSounds, m_coins, m_enemyProjectiles, m_mapBounds);

        for (auto &effect: m_effects) effect->update();

        // Magnet monede si colectare
        const float magnetRadius = 150.f;
        const float magnetRadiusSq = magnetRadius * magnetRadius;
        const float magnetSpeed = 500.f;
        sf::Vector2f playerPos = m_player->getPosition();

        for (auto &coin: m_coins) {
            coin->update();
            sf::Vector2f coinPos = coin->getPosition();
            sf::Vector2f dir = playerPos - coinPos;
            float distSq = dir.x * dir.x + dir.y * dir.y;
            if (distSq < magnetRadiusSq && distSq > 0.01f) {
                float dist = std::sqrt(distSq);
                coin->setPosition(coinPos + (dir / dist) * magnetSpeed * dt.asSeconds());
            }
        }
        std::erase_if(m_coins, [&](const auto &coin) {
            if (m_player->getCollisionBounds().findIntersection(coin->getBounds())) {
                m_activeSounds.emplace_back(m_soundBuffers["coin_pickup"]);
                m_activeSounds.back().setPitch(RandomGenerator::getFloat(1.1f, 1.3f));
                m_activeSounds.back().setVolume(100.f);
                m_activeSounds.back().play();
                m_player->addCoins(1);
                return true;
            }
            return coin->isDead();
        });

        // Logica proiectilelor
        for (auto &proj: m_enemyProjectiles) {
            proj->update(dt.asSeconds(), m_gameMap);
            if (proj->getBounds().findIntersection(m_player->getCollisionBounds()) &&
                m_playerDamageTimer.getElapsedTime().asSeconds() > m_playerIframeDuration) {
                sf::Vector2f knockbackDir = m_player->getPosition() - proj->getBounds().position;
                float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
                if (length != 0.f) knockbackDir /= length; else knockbackDir = {1.f, 0.f};

                m_player->takeDamage(15.f, knockbackDir);
                m_activeSounds.emplace_back(m_soundBuffers["player_hurt"]);
                m_activeSounds.back().setPitch(RandomGenerator::getFloat(0.5f, 1.2f));
                m_activeSounds.back().setVolume(80.f);
                m_activeSounds.back().play();
                m_playerDamageTimer.restart();
                proj->hit();
            }
        }
        std::erase_if(m_enemyProjectiles, [](const auto &proj) { return proj->isDead(); });

        // Atacuri melee inamici
        // Pot accesa inamicii prin getter-ul EnemyManager
        const auto& enemies = m_enemyManager.getEnemies();
        for (const auto& enemy : enemies) {
            if (enemy->isDead()) continue;
            if (enemy->didAttackLand()) {
                sf::FloatRect attackBox = enemy->getAttackHitbox();
                if (attackBox.size.x >= 0) { // Nu este un atac proiectil
                    if (attackBox.findIntersection(m_player->getCollisionBounds()) &&
                       m_playerDamageTimer.getElapsedTime().asSeconds() > m_playerIframeDuration) {
                        sf::Vector2f knockbackDir = m_player->getPosition() - enemy->getPosition();
                        float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
                        if (length != 0.f) knockbackDir /= length; else knockbackDir = {1.f, 0.f};

                        m_player->takeDamage(10.f, knockbackDir);
                        m_activeSounds.emplace_back(m_soundBuffers["player_hurt"]);
                        m_activeSounds.back().setPitch(RandomGenerator::getFloat(0.5f, 1.2f));
                        m_activeSounds.back().setVolume(80.f);
                        m_activeSounds.back().play();
                        m_playerDamageTimer.restart();

                        if (dynamic_cast<ChaserEnemy *>(enemy.get())) {
                            m_activeSounds.emplace_back(m_soundBuffers["chaser_attack"]);
                            m_activeSounds.back().setVolume(20.f);
                            m_activeSounds.back().play();
                        } else if (dynamic_cast<GhostEnemy *>(enemy.get())) {
                            m_activeSounds.emplace_back(m_soundBuffers["ghost_attack"]);
                            m_activeSounds.back().setVolume(20.f);
                            m_activeSounds.back().play();
                        }
                    }
                }
            }
        }

        // Camera
        sf::Vector2f viewSizeBlocked = m_camera.getSize();
        sf::Vector2f viewCenter = playerPos;
        float halfViewX = viewSizeBlocked.x / 2.0f;
        float halfViewY = viewSizeBlocked.y / 2.0f;
        if (viewCenter.x < halfViewX) viewCenter.x = halfViewX;
        if (viewCenter.x > m_mapBounds.size.x - halfViewX) viewCenter.x = m_mapBounds.size.x - halfViewX;
        if (viewCenter.y < halfViewY) viewCenter.y = halfViewY;
        if (viewCenter.y > m_mapBounds.size.y - halfViewY) viewCenter.y = m_mapBounds.size.y - halfViewY;
        m_camera.setCenter(viewCenter);

        if (m_player->isDead()) {
            m_state = GameState::GameOver;
            m_backgroundMusic.stop();
        }

        // Curatare
        m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(), 
            [](const Bullet &b) { return b.isDead(); }), m_bullets.end());
        std::erase_if(m_effects, [](const auto &e) { return e->isDead(); });
    } 
    else if (m_state == GameState::Paused) {
        m_upgradeMenu.update(*m_player);
    }
    
    std::erase_if(m_activeSounds, [](const sf::Sound &s) {
        return s.getStatus() == sf::Sound::Status::Stopped;
    });
}

void Game::render() {
    m_window.clear(sf::Color(30, 30, 30));

    if (m_state == GameState::Playing || m_state == GameState::Paused) {
        // Desenez lumea
        m_window.setView(m_camera);
        m_window.draw(m_gameMap);
        m_gameMap.updateAndDrawCooldowns(m_window);

        for (const auto &coin: m_coins) coin->draw(m_window);
        m_player->drawWorld(m_window);
        m_enemyManager.draw(m_window);
        for (const auto &effect: m_effects) effect->draw(m_window);
        for (const auto &proj: m_enemyProjectiles) proj->draw(m_window);
        for (auto &bullet: m_bullets) bullet.draw(m_window);

        // Desenez UI
        m_window.setView(m_uiView);
        
        std::string ammoString = std::to_string(m_player->getCurrentAmmo()) + " / " + std::to_string(m_player->getReserveAmmo());
        std::string weaponNameString = m_player->getCurrentWeaponName();
        m_ammoText.setString(weaponNameString + '\n' + ammoString);

        sf::Vector2f viewSize = static_cast<sf::Vector2f>(m_window.getSize());
        float totalTextHeight = (static_cast<float>(m_ammoText.getCharacterSize()) * 2.f) * 1.1f;
        m_ammoText.setPosition({10.f, viewSize.y - totalTextHeight - 10.f});

        m_coinText.setString(std::to_string(m_player->getCoinCount()));
        float coinTextWidth = m_coinText.getGlobalBounds().size.x;
        float coinIconWidth = m_coinIconSprite.getGlobalBounds().size.x;
        m_coinIconSprite.setPosition({viewSize.x - coinIconWidth - 100.f, 15.f});
        m_coinText.setPosition({viewSize.x - coinTextWidth - 20.f, 10.f});

        m_player->drawUI(m_window);
        m_window.draw(m_ammoText);
        m_window.draw(m_coinText);
        m_window.draw(m_coinIconSprite);

        if (m_state == GameState::Playing) {
            sf::Vector2i mousePositionWindow = sf::Mouse::getPosition(m_window);
            m_crosshairSprite.setPosition(static_cast<sf::Vector2f>(mousePositionWindow));
            m_window.draw(m_crosshairSprite);
        }

        if (m_state == GameState::Paused) {
            m_upgradeMenu.draw(m_window, m_uiView);
        }
    }
    else if (m_state == GameState::MainMenu) {
        m_window.setView(m_uiView);
        sf::Vector2f viewSize = m_uiView.getSize();
        m_menuTitle.setPosition({viewSize.x / 2.f - m_menuTitle.getGlobalBounds().size.x / 2.f, 200.f});
        m_menuStart.setPosition({viewSize.x / 2.f - m_menuStart.getGlobalBounds().size.x / 2.f, 350.f});
        m_menuExit.setPosition({viewSize.x / 2.f - m_menuExit.getGlobalBounds().size.x / 2.f, 420.f});
        m_window.draw(m_menuTitle);
        m_window.draw(m_menuStart);
        m_window.draw(m_menuExit);
    }
    else if (m_state == GameState::GameOver) {
        m_window.setView(m_uiView);
        sf::Vector2f viewSize = m_uiView.getSize();
        m_gameOverTitle.setPosition({viewSize.x / 2.f - m_gameOverTitle.getGlobalBounds().size.x / 2.f, 200.f});
        m_gameOverRestart.setPosition({viewSize.x / 2.f - m_gameOverRestart.getGlobalBounds().size.x / 2.f, 350.f});
        m_window.draw(m_gameOverTitle);
        m_window.draw(m_gameOverRestart);
    }

    m_window.display();
}
