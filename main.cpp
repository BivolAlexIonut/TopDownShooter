#include <cmath>
#include <iostream>
#include <ostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <map>
#include <list>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "ChaserEnemy.h"
#include "GameMap.h"
#include "Player.h"
#include "RandomGenerator.h"
#include "Effect.h"
#include "GhostEnemy.h"
#include "GameExceptions.h"
#include "Coin.h"
#include "DevilEnemy.h"
#include "DevilProjectile.h"
#include "UpgradeMenu.h"

std::map<std::string, sf::SoundBuffer> soundBuffers;
std::list<sf::Sound> activeSounds;
sf::Music backgroundMusic;

enum class GameState { MainMenu, Playing, Paused, GameOver };
GameState currentState = GameState::MainMenu;

UpgradeMenu upgradeMenu;

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Top-Down Shooter");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);
    sf::Texture crosshairTexture;
    if (!crosshairTexture.loadFromFile("assets/ui/crosshair.png")) {
        std::cerr << "NU AM PUTUT INCARCA ui/crosshair.png" << std::endl;
    }
    sf::Sprite crosshairSprite(crosshairTexture);
    crosshairSprite.setTextureRect(sf::IntRect({static_cast<int>(20.f), static_cast<int>(30.f)}, {13, 13}));
    crosshairSprite.setOrigin({6.5f, 6.5f});
    crosshairSprite.setScale(sf::Vector2f(2.f, 2.f));

    GameMap gameMap;
    sf::FloatRect mapBounds;

    sf::Font ammoFont;

    const std::string devilBasePath = "assets/enemies/Flying Demon 2D Pixel Art/Sprites/with_outline/";
    const std::string devilProjectilePath = "assets/enemies/Flying Demon 2D Pixel Art/Sprites/projectile.png";

    std::unique_ptr<Player> player;

    try {
        constexpr float mapScale = 0.4f;
        if (!gameMap.load("assets/Levels/level1.txt",
                          "assets/Premium Content/Tileset with cell size 256x256.png", mapScale)) {
            throw MapLoadException("level1.txt", "Functia load a returnat false.");
        }

        mapBounds = gameMap.getPixelBounds();
        ChaserEnemy::initAssets();
        DevilEnemy::initAssets(devilBasePath);
        DevilProjectile::initAssets(devilProjectilePath);
        GhostEnemy::initAssets();
        Coin::initAssets();

        if (!ammoFont.openFromFile("fonts/m6x11.ttf")) {
            throw FontLoadException("fonts/m6x11.ttf");
        }
        if (!upgradeMenu.loadFont("fonts/m6x11.ttf")) {
            throw FontLoadException("m6x11.ttf pentru meniu");
        }
        std::cout << "Loading sounds..." << std::endl;
        if (!soundBuffers["player_step"].loadFromFile("assets/sounds/steps/Steps_floor-001.wav")) {
            throw AssetLoadException("Steps_floor-001.wav");
        }
        if (!soundBuffers["pistol_shoot"].loadFromFile("assets/sounds/shoot/pistol.wav")) {
            throw AssetLoadException("pistol.ogg");
        }
        if (!soundBuffers["chaser_hurt"].loadFromFile(
            "assets/sounds/enemies/chaser/Orc_Damage.wav")) {
            throw AssetLoadException("Orc_Damage.wav");
        }
        if (!soundBuffers["ghost_hurt"].loadFromFile("assets/sounds/enemies/ghost/Ghost_Death.wav")) {
            throw AssetLoadException("Ghost_Death.wav");
        }
        if (!soundBuffers["devil_hurt"].loadFromFile("assets/sounds/enemies/devil/Behemoth_Death.wav")) {
            throw AssetLoadException("Behemoth_Death.wav");
        }
        if (!soundBuffers["chaser_attack"].loadFromFile("assets/sounds/enemies/chaser/Attack.wav")) {
            throw AssetLoadException("Chaser_Attack.wav");
        }
        if (!soundBuffers["ghost_attack"].loadFromFile("assets/sounds/enemies/ghost/Attack.wav")) {
            throw AssetLoadException("Ghost_Attack.wav");
        }
        if (!soundBuffers["devil_attack"].loadFromFile("assets/sounds/enemies/devil/Attack.wav")) {
            throw AssetLoadException("Devil_Attack.wav");
        }
        if (!soundBuffers["chaser_move"].loadFromFile("assets/sounds/enemies/chaser/Move.wav")) {
            throw AssetLoadException("Chaser_Move.ogg");
        }
        if (!soundBuffers["ghost_move"].loadFromFile("assets/sounds/enemies/chaser/Move.wav")) {
            throw AssetLoadException("Chaser_Move.ogg");
        }
        if (!soundBuffers["devil_move"].loadFromFile("assets/sounds/enemies/chaser/Move.wav")) {
            throw AssetLoadException("Chaser_Move.ogg");
        }
        if (!soundBuffers["pistol_shoot"].loadFromFile("assets/sounds/shoot/pistol.wav")) {
            throw AssetLoadException("pistol.wav");
        }
        if (!soundBuffers["pistol_reload"].loadFromFile("assets/sounds/reload/pistol_reload.wav")) {
            throw AssetLoadException("pistol_reload.wav");
        }
        if (!soundBuffers["tommygun_shoot"].loadFromFile("assets/sounds/shoot/tommygun.wav")) {
            throw AssetLoadException("tommygun.wav");
        }
        if (!soundBuffers["tommygun_reload"].loadFromFile("assets/sounds/reload/tommygun_reload.wav")) {
            throw AssetLoadException("tommygun_reload.wav");
        }
        if (!soundBuffers["rpg_shoot"].loadFromFile("assets/sounds/shoot/rpg.wav")) {
            throw AssetLoadException("rpg.wav");
        }
        if (!soundBuffers["rpg_reload"].loadFromFile("assets/sounds/reload/rpg_reload.wav")) {
            throw AssetLoadException("rpg_reload.wav");
        }
        if (!soundBuffers["smg_shoot"].loadFromFile("assets/sounds/shoot/smg.wav")) {
            throw AssetLoadException("smg.wav");
        }
        if (!soundBuffers["smg_reload"].loadFromFile("assets/sounds/reload/smg_reload.wav")) {
            throw AssetLoadException("smg_reload.wav");
        }
        if (!soundBuffers["shotgun_shoot"].loadFromFile("assets/sounds/shoot/shotgun.wav")) {
            throw AssetLoadException("shotgun.wav");
        }
        if (!soundBuffers["shotgun_reload"].loadFromFile("assets/sounds/reload/shotgun_reload.wav")) {
            throw AssetLoadException("shotgun_reload.wav");
        }
        if (!soundBuffers["sniper_shoot"].loadFromFile("assets/sounds/shoot/sniper.wav")) {
            throw AssetLoadException("sniper.wav");
        }
        if (!soundBuffers["sniper_reload"].loadFromFile("assets/sounds/reload/sniper_reload.wav")) {
            throw AssetLoadException("sniper_reload.wav");
        }
        if (!soundBuffers["coin_drop"].loadFromFile("assets/sounds/coin_drop.wav")) {
            throw AssetLoadException("coin_drop.wav");
        }
        if (!soundBuffers["coin_pickup"].loadFromFile("assets/sounds/coin_pickup.wav")) {
            throw AssetLoadException("coin_pickup.wav");
        }
        if (!backgroundMusic.openFromFile("assets/sounds/background/retro_metal.ogg")) {
            throw AssetLoadException("retro_metal.ogg");
        }
        if (!soundBuffers["player_hurt"].loadFromFile("assets/sounds/player/damage.wav")) {
            throw AssetLoadException("damage.wav");
        }

    } catch (const GameException &e) {
        std::cerr << "EROARE FATALA: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << "EROARE NECUNOSCUTA: " << e.what() << std::endl;
        return -1;
    }

    backgroundMusic.setVolume(60.f);
    backgroundMusic.setLooping(true);

    std::vector<std::unique_ptr<EnemyBase> > enemies;
    constexpr int MAX_ENEMIES = 10;
    const float RESPAWN_DELAY = 6.0f;
    sf::Clock respawnTimer;

    sf::Clock clock;
    sf::View camera;
    camera.setSize({1280, 720});

    sf::View uiView;
    uiView.setSize(static_cast<sf::Vector2f>(window.getSize()));
    uiView.setCenter(static_cast<sf::Vector2f>(window.getSize()) / 2.f);

    std::vector<Bullet> bullets;
    std::vector<std::unique_ptr<Effect> > effects;
    std::vector<std::unique_ptr<Coin> > coins;
    std::vector<std::unique_ptr<DevilProjectile> > enemyProjectiles;
    sf::Clock shootTimer;

    sf::Text ammoText(ammoFont);
    ammoText.setFont(ammoFont);
    ammoText.setCharacterSize(48);
    ammoText.setFillColor(sf::Color::Black);

    sf::Clock playerDamageTimer;
    constexpr float PLAYER_IFRAME_DURATION = 0.3f;
    sf::RectangleShape debugHitbox;

    sf::Texture coinIconTexture;
    if (!coinIconTexture.loadFromFile("assets/coin.png")) {
        throw AssetLoadException("coin.png");
    }
    sf::Sprite coinIconSprite(coinIconTexture);
    coinIconSprite.setTextureRect(sf::IntRect({0, 0}, {20, 20}));
    coinIconSprite.setScale({1.5f, 1.5f});

    sf::Text coinText(ammoFont);
    coinText.setFont(ammoFont);
    coinText.setCharacterSize(48);
    coinText.setFillColor(sf::Color(240, 180, 0));

    sf::Text menuTitle(ammoFont);
    menuTitle.setString("TOP-DOWN SHOOTER");
    menuTitle.setFont(ammoFont);
    menuTitle.setCharacterSize(72);
    menuTitle.setFillColor(sf::Color::White);

    sf::Text menuStart(ammoFont);
    menuStart.setString("Apasati ENTER pentru a incepe");
    menuStart.setFont(ammoFont);
    menuStart.setCharacterSize(48);
    menuStart.setFillColor(sf::Color::Yellow);

    sf::Text menuExit(ammoFont);
    menuExit.setString("Apasati ESCAPE pentru a iesi");
    menuExit.setFont(ammoFont);
    menuExit.setCharacterSize(48);
    menuExit.setFillColor(sf::Color::White);

    sf::Text gameOverTitle(ammoFont);
    gameOverTitle.setString("GAME OVER");
    gameOverTitle.setFont(ammoFont);
    gameOverTitle.setCharacterSize(72);
    gameOverTitle.setFillColor(sf::Color::Red);

    sf::Text gameOverRestart(ammoFont);
    gameOverRestart.setString("Apasati R pentru a reveni la meniu");
    gameOverRestart.setFont(ammoFont);
    gameOverRestart.setCharacterSize(48);
    gameOverRestart.setFillColor(sf::Color::Yellow);


    sf::Texture ghostImpactTexture;
    if (!ghostImpactTexture.loadFromFile("assets/enemies/death_animation-ghost.png")) {
        std::cerr << "EROARE la incarcarea ghost_impact_animation.png pentru impact";
        return -1;
    }
    std::vector<sf::IntRect> ghostImpactFrames;
    const int GHOST_IMPACT_W = 256;
    const int GHOST_IMPACT_H = 256;

    const int GHOST_IMPACT_COLS = 5;
    const int GHOST_IMPACT_TOTAL_FRAMES = 20;

    for (int i = 0; i < GHOST_IMPACT_TOTAL_FRAMES; ++i) {
        int row = i / GHOST_IMPACT_COLS;
        int col = i % GHOST_IMPACT_COLS;
        ghostImpactFrames.push_back(sf::IntRect(
            {col * GHOST_IMPACT_W, row * GHOST_IMPACT_H},
            {GHOST_IMPACT_W, GHOST_IMPACT_H}
        ));
    }

    sf::Texture bloodEffectTexture;
    if (!bloodEffectTexture.loadFromFile("assets/enemies/death_animation.png")) {
        std::cerr << "EROARE la incarcarea death_animation.png";
        return -1;
    }
    std::vector<sf::IntRect> bloodEffectFrames;
    const int BLOOD_FRAME_WIDTH = 256;
    const int BLOOD_FRAME_HEIGHT = 256;
    const int BLOOD_COLS = 5;
    const int BLOOD_ROWS = 4;

    for (int y = 0; y < BLOOD_ROWS; ++y) {
        for (int x = 0; x < BLOOD_COLS; ++x) {
            bloodEffectFrames.push_back(sf::IntRect({x * BLOOD_FRAME_WIDTH, y * BLOOD_FRAME_HEIGHT},
                                                    {BLOOD_FRAME_WIDTH, BLOOD_FRAME_HEIGHT}));
        }
    }

    while (window.isOpen()) {
        sf::Time dt = clock.restart();

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
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

                camera.setViewport(sf::FloatRect({viewportX, viewportY}, {viewportWidth, viewportHeight}));
                uiView.setSize({newWidth, newHeight});
                uiView.setCenter({newWidth / 2.f, newHeight / 2.f});
            }


            if (event->is<sf::Event::KeyPressed>()) {
                if (currentState == GameState::MainMenu) {
                    if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Enter) {
                        player.reset();
                        constexpr float mapScale = 0.4f;
                        player = std::make_unique<Player>(1640 * mapScale, 1360 * mapScale, soundBuffers);

                        enemies.clear();
                        bullets.clear();
                        effects.clear();
                        coins.clear();
                        enemyProjectiles.clear();

                        for (int i = 0; i < MAX_ENEMIES; ++i) {
                            sf::Vector2f randomPos;
                            do {
                                float x = RandomGenerator::getFloat(mapBounds.position.x, mapBounds.position.x + mapBounds.size.x);
                                float y = RandomGenerator::getFloat(mapBounds.position.y, mapBounds.position.y + mapBounds.size.y);
                                randomPos = {x, y};
                            } while (gameMap.isSolid(randomPos));

                            float randType = RandomGenerator::getFloat(0.f, 1.f);
                            if (randType < 0.2f) {
                                enemies.push_back(std::make_unique<ChaserEnemy>(soundBuffers));
                            } else if (randType < 0.4f) {
                                enemies.push_back(std::make_unique<GhostEnemy>(soundBuffers));
                            } else {
                                enemies.push_back(std::make_unique<DevilEnemy>(soundBuffers));
                            }
                            enemies.back()->setPosition(randomPos);
                        }

                        camera.setCenter(player->getPosition());

                        clock.restart();
                        respawnTimer.restart();
                        shootTimer.restart();
                        playerDamageTimer.restart();

                        backgroundMusic.play();
                        currentState = GameState::Playing;
                    }
                    if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
                }
                else if (currentState == GameState::GameOver) {
                    if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::R) {
                        currentState = GameState::MainMenu;
                    }
                    if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
                }
                else {
                    if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Tab) {
                        if (currentState == GameState::Playing) {
                            currentState = GameState::Paused;
                            upgradeMenu.setDisplayedWeapon(player->getCurrentWeaponIndex());
                        } else {
                            currentState = GameState::Playing;
                        }
                    }
                    if (currentState == GameState::Paused) {
                        if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Backspace) {
                           window.close();
                        }
                        upgradeMenu.handleInput(event->getIf<sf::Event::KeyPressed>()->code, *player);
                    }
                    else if (currentState == GameState::Playing) {
                        if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::E) {
                            player->switchWeaponNext();
                        }
                        if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Q) {
                            player->switchWeaponPrev();
                        }
                        if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::R) {
                            std::pair<std::string, float> reloadData = player->reload();

                            std::string reloadSoundKey = reloadData.first;
                            float reloadAnimDuration = reloadData.second;

                            if (!reloadSoundKey.empty()) {
                                activeSounds.emplace_back(soundBuffers[reloadSoundKey]);
                                sf::Sound &newSound = activeSounds.back();

                                float soundOriginalDuration = soundBuffers[reloadSoundKey].getDuration().asSeconds();
                                float newPitch = 1.0f;

                                if (reloadAnimDuration > 0.01f && soundOriginalDuration > 0.01f) {
                                    newPitch = soundOriginalDuration / reloadAnimDuration;
                                }

                                newSound.setPitch(newPitch);
                                newSound.play();
                            }
                        }
                        if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Backspace) {
                            window.close();
                        }
                    }
                }
            }
        }

        sf::Vector2i mousePositionWindow = sf::Mouse::getPosition(window);
        if (currentState == GameState::Playing)
        {
            sf::Vector2i mousePositionWindow2 = sf::Mouse::getPosition(window);
            sf::Vector2f mousePositionWorld = window.mapPixelToCoords(mousePositionWindow2, camera);

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && shootTimer.getElapsedTime().asSeconds() > player->
                getCurrentWeaponCooldown()
                && player->canShoot(mousePositionWorld)) {
                bullets.push_back((player->shoot(mousePositionWorld)));
                std::string shootSoundKey = player->getShootSoundKey();

                if (!shootSoundKey.empty()) {
                    activeSounds.emplace_back(soundBuffers[shootSoundKey]);
                    activeSounds.back().setPitch(RandomGenerator::getFloat(0.95f, 1.05f));
                    activeSounds.back().play();
                }
                shootTimer.restart();
            }

            player->update(dt.asSeconds(), mousePositionWorld, gameMap);

            for (auto &bullet: bullets) {
                bullet.update(dt.asSeconds());
                if (!bullet.isImpacting()) {
                    for (auto &enemy: enemies) {
                        if (enemy->isDead()) continue;

                        if (enemy->getBounds().findIntersection(bullet.getBounds())) {
                            float damage = bullet.getDamage();
                            enemy->takeDamage(damage);

                            if (dynamic_cast<ChaserEnemy *>(enemy.get())) {
                                activeSounds.emplace_back(soundBuffers["chaser_hurt"]);
                                activeSounds.back().setPitch(RandomGenerator::getFloat(0.9f, 1.1f));
                                activeSounds.back().setVolume(20.f);
                                activeSounds.back().play();
                            } else if (dynamic_cast<GhostEnemy *>(enemy.get())) {
                                activeSounds.emplace_back(soundBuffers["ghost_hurt"]);
                                activeSounds.back().setPitch(RandomGenerator::getFloat(1.1f, 1.3f));
                                activeSounds.back().setVolume(20.f);
                                activeSounds.back().play();
                            } else if (dynamic_cast<DevilEnemy *>(enemy.get())) {
                                activeSounds.emplace_back(soundBuffers["devil_hurt"]);
                                activeSounds.back().setPitch(RandomGenerator::getFloat(3.0f, 4.0f));
                                activeSounds.back().setVolume(20.f);
                                activeSounds.back().play();
                            }
                            bullet.hit();

                            if (dynamic_cast<GhostEnemy *>(enemy.get())) {
                                effects.push_back(std::make_unique<Effect>(
                                    ghostImpactTexture,
                                    ghostImpactFrames,
                                    bullet.getPosition(),
                                    0.03f,
                                    sf::Vector2f(0.6f, 0.6f)
                                ));
                            } else {
                                float effectScale = 0.25f + (damage / 200.f);
                                effectScale = std::clamp(effectScale, 0.2f, 2.0f);
                                effects.push_back(std::make_unique<Effect>(
                                    bloodEffectTexture,
                                    bloodEffectFrames,
                                    bullet.getPosition(),
                                    0.01f,
                                    sf::Vector2f(effectScale, effectScale)
                                ));
                            }

                            break;
                        }
                    }
                }
            }

            if (respawnTimer.getElapsedTime().asSeconds() > RESPAWN_DELAY) {
                if (enemies.size() < MAX_ENEMIES) {
                    int enemiesToSpawn = MAX_ENEMIES - static_cast<int>(enemies.size());

                    for (int i = 0; i < enemiesToSpawn; ++i) {
                        sf::Vector2f randomPos;
                        do {
                            float x = RandomGenerator::getFloat(mapBounds.position.x,
                                                                mapBounds.position.x + mapBounds.size.x);
                            float y = RandomGenerator::getFloat(mapBounds.position.y,
                                                                mapBounds.position.y + mapBounds.size.y);
                            randomPos = {x, y};
                        } while (gameMap.isSolid(randomPos));

                        float randType = RandomGenerator::getFloat(0.f, 1.f);
                        if (randType < 0.2f) {
                            enemies.push_back(std::make_unique<ChaserEnemy>(soundBuffers));
                        } else if (randType < 0.4f) {
                            enemies.push_back(std::make_unique<GhostEnemy>(soundBuffers));
                        } else {
                            enemies.push_back(std::make_unique<DevilEnemy>(soundBuffers));
                        }
                        enemies.back()->setPosition(randomPos);
                    }
                }
                respawnTimer.restart();
            }

            for (auto &enemy: enemies) {
                if (!enemy->isDead()) {
                    enemy->update(dt, player->getPosition(), gameMap);
                }
            }

            for (auto &effect: effects) {
                effect->update();
            }


            const float magnetRadius = 150.f;
            const float magnetRadiusSq = magnetRadius * magnetRadius;
            const float magnetSpeed = 500.f;
            sf::Vector2f playerPos = player->getPosition();
            float dt_sec = dt.asSeconds();

            for (auto &coin: coins) {
                coin->update();

                sf::Vector2f coinPos = coin->getPosition();
                sf::Vector2f dir = playerPos - coinPos;
                float distSq = dir.x * dir.x + dir.y * dir.y;

                if (distSq < magnetRadiusSq && distSq > 0.01f) {
                    float dist = std::sqrt(distSq);
                    sf::Vector2f normDir = dir / dist;
                    sf::Vector2f velocity = normDir * magnetSpeed * dt_sec;

                    coin->setPosition(coinPos + velocity);
                }
            }
            std::erase_if(coins, [&](const auto &coin) {
                if (player->getCollisionBounds().findIntersection(coin->getBounds())) {
                    activeSounds.emplace_back(soundBuffers["coin_pickup"]);
                    activeSounds.back().setPitch(RandomGenerator::getFloat(1.1f, 1.3f));
                    activeSounds.back().setVolume(100.f);
                    activeSounds.back().play();

                    player->addCoins(1);
                    return true;
                }
                return coin->isDead();
            });

            for (auto &enemy: enemies) {
                if (enemy->hasJustDied()) {
                    activeSounds.emplace_back(soundBuffers["coin_drop"]);
                    activeSounds.back().setPitch(RandomGenerator::getFloat(0.9f, 1.1f));
                    activeSounds.back().setVolume(100.f);
                    activeSounds.back().play();

                    int coinCount = enemy->getCoinValue();
                    for (int c = 0; c < coinCount; ++c) {
                        sf::Vector2f pos = enemy->getPosition();
                        pos.x += RandomGenerator::getFloat(-40.f, 40.f);
                        pos.y += RandomGenerator::getFloat(-40.f, 40.f);
                        coins.push_back(std::make_unique<Coin>(pos));
                    }
                    enemy->acknowledgeDeath();
                }
            }

            for (auto &proj: enemyProjectiles) {
                proj->update(dt.asSeconds(), gameMap);

                if (proj->getBounds().findIntersection(player->getCollisionBounds()) &&
                    playerDamageTimer.getElapsedTime().asSeconds() > PLAYER_IFRAME_DURATION) {
                    sf::Vector2f knockbackDir = player->getPosition() - proj->getBounds().position;
                    float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
                    if (length != 0.f) knockbackDir /= length;
                    else knockbackDir = {1.f, 0.f};

                    player->takeDamage(15.f, knockbackDir);
                    activeSounds.emplace_back(soundBuffers["player_hurt"]);
                    activeSounds.back().setPitch(RandomGenerator::getFloat(0.5f, 1.2f));
                    activeSounds.back().setVolume(80.f);
                    activeSounds.back().play();
                    playerDamageTimer.restart();
                    proj->hit();
                }
            }
            std::erase_if(enemyProjectiles, [](const auto &proj) { return proj->isDead(); });

            for (auto &enemy: enemies) {
                if (enemy->isDead()) continue;

                if (enemy->didAttackLand()) {
                    sf::FloatRect attackBox = enemy->getAttackHitbox();

                    if (attackBox.size.x < 0) {
                        sf::Vector2f spawnPos = enemy->getPosition();
                        sf::Vector2f dir = player->getPosition() - spawnPos;
                        enemyProjectiles.push_back(std::make_unique<DevilProjectile>(spawnPos, dir));

                        activeSounds.emplace_back(soundBuffers["devil_attack"]);
                        activeSounds.back().setPitch(2.f);
                        activeSounds.back().play();
                    } else if (attackBox.findIntersection(player->getCollisionBounds()) &&
                               playerDamageTimer.getElapsedTime().asSeconds() > PLAYER_IFRAME_DURATION) {
                        sf::Vector2f knockbackDir = player->getPosition() - enemy->getPosition();
                        float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
                        if (length != 0.f) {
                            knockbackDir /= length;
                        } else {
                            knockbackDir = {1.f, 0.f};
                        }
                        player->takeDamage(10.f, knockbackDir);
                        activeSounds.emplace_back(soundBuffers["player_hurt"]);
                        activeSounds.back().setPitch(RandomGenerator::getFloat(0.5f, 1.2f));
                        activeSounds.back().setVolume(80.f);
                        activeSounds.back().play();
                        playerDamageTimer.restart();
                        if (dynamic_cast<ChaserEnemy *>(enemy.get())) {
                            activeSounds.emplace_back(soundBuffers["chaser_attack"]);
                            activeSounds.back().setVolume(20.f);
                            activeSounds.back().play();
                        } else if (dynamic_cast<GhostEnemy *>(enemy.get())) {
                            activeSounds.emplace_back(soundBuffers["ghost_attack"]);
                            activeSounds.back().setVolume(20.f);
                            activeSounds.back().play();
                        }

                        break;
                    }
                }
            }

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

            if (player->isDead()) {
                currentState = GameState::GameOver;
                backgroundMusic.stop();
            }
        }
        else if (currentState == GameState::Paused)
        {
            upgradeMenu.update(*player);
        }

        window.clear(sf::Color(30, 30, 30));

        if (currentState == GameState::Playing || currentState == GameState::Paused) {
            std::string ammoString = std::to_string(player->getCurrentAmmo()) + " / " + std::to_string(
                                         player->getReserveAmmo());
            std::string weaponNameString = player->getCurrentWeaponName();
            std::string uiText = weaponNameString + '\n';
            std::string ammoFinal = uiText + ammoString;
            ammoText.setString(ammoFinal);

            window.setView(camera);
            window.draw(gameMap);
            gameMap.updateAndDrawCooldowns(window);

            for (const auto &coin: coins) {
                coin->draw(window);
            }

            player->drawWorld(window);

            for (auto &enemy: enemies) {
                enemy->draw(window);
            }

            for (const auto &effect: effects) {
                effect->draw(window);
            }

            for (const auto &proj: enemyProjectiles) {
                proj->draw(window);
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
            std::erase_if(effects, [](const auto &effect) {
                return effect->isDead();
            });
            std::erase_if(enemies, [&](const auto &enemy) {
                if (enemy->isDead()) {
                    return true;
                }
                return false;
            });

            window.setView(uiView);

            crosshairSprite.setPosition(static_cast<sf::Vector2f>(mousePositionWindow));
            ammoText.setOrigin({0.f, 0.f});

            sf::Vector2f viewSize = static_cast<sf::Vector2f>(window.getSize());

            auto charSize = static_cast<float>(ammoText.getCharacterSize());
            float numLines = 2.f;
            float totalTextHeight = (charSize * numLines) * 1.1f;
            float positionY = viewSize.y - totalTextHeight - 10.f;
            ammoText.setPosition({10.f, positionY});

            coinText.setString(std::to_string(player->getCoinCount()));

            float coinTextWidth = coinText.getGlobalBounds().size.x;
            float coinIconWidth = coinIconSprite.getGlobalBounds().size.x;

            coinIconSprite.setPosition({viewSize.x - coinIconWidth - 100.f, 15.f});
            coinText.setPosition({viewSize.x - coinTextWidth - 20.f, 10.f});

            player->drawUI(window);
            window.draw(ammoText);
            window.draw(coinText);
            window.draw(coinIconSprite);

            if (currentState == GameState::Playing) {
                window.draw(crosshairSprite);
            }

            if (currentState == GameState::Paused) {
                upgradeMenu.draw(window, uiView);
            }
        }
        else if (currentState == GameState::MainMenu) {
            window.setView(uiView);
            sf::Vector2f viewSize = uiView.getSize();
            menuTitle.setPosition({viewSize.x / 2.f - menuTitle.getGlobalBounds().size.x / 2.f, 200.f});
            menuStart.setPosition({viewSize.x / 2.f - menuStart.getGlobalBounds().size.x / 2.f, 350.f});
            menuExit.setPosition({viewSize.x / 2.f - menuExit.getGlobalBounds().size.x / 2.f, 420.f});

            window.draw(menuTitle);
            window.draw(menuStart);
            window.draw(menuExit);
        }
        else if (currentState == GameState::GameOver) {
            window.setView(uiView);
            sf::Vector2f viewSize = uiView.getSize();
            gameOverTitle.setPosition({viewSize.x / 2.f - gameOverTitle.getGlobalBounds().size.x / 2.f, 200.f});
            gameOverRestart.setPosition({viewSize.x / 2.f - gameOverRestart.getGlobalBounds().size.x / 2.f, 350.f});

            window.draw(gameOverTitle);
            window.draw(gameOverRestart);
        }

        std::erase_if(activeSounds, [](const sf::Sound &s) {
            return s.getStatus() == sf::Sound::Status::Stopped;
        });

        window.display();
    }

    return 0;
}