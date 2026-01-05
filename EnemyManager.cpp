#include "EnemyManager.h"
#include "ChaserEnemy.h"
#include "GhostEnemy.h"
#include "DevilEnemy.h"
#include "RandomGenerator.h"
#include <algorithm>

EnemyManager::EnemyManager(std::map<std::string, sf::SoundBuffer>& soundBuffers)
    : m_soundBuffers(soundBuffers)
{
}

void EnemyManager::reset() {
    m_enemies.clear();
    m_respawnTimer.restart();
}

void EnemyManager::update(float dt, const Player& player, const GameMap& map,
                          std::list<sf::Sound>& activeSounds,
                          std::vector<std::unique_ptr<Coin>>& coins,
                          std::vector<std::unique_ptr<DevilProjectile>>& enemyProjectiles,
                          const sf::FloatRect& mapBounds) {

    // Logica de spawnare
    if (m_respawnTimer.getElapsedTime().asSeconds() > m_respawnDelay) {
        if (m_enemies.size() < m_maxEnemies) {
            int enemiesToSpawn = static_cast<int>(m_maxEnemies) - static_cast<int>(m_enemies.size());
            for (int i = 0; i < enemiesToSpawn; ++i) {
                spawnEnemy(mapBounds, map);
            }
        }
        m_respawnTimer.restart();
    }

    // Actualizare inamici
    for (auto& enemy : m_enemies) {
        if (!enemy->isDead()) {
            enemy->update(sf::seconds(dt), player.getPosition(), map);

            // Gestioneaza atacurile care creeaza proiectile (DevilEnemy)
            if (enemy->didAttackLand()) {
                sf::FloatRect attackBox = enemy->getAttackHitbox();
                // Logica DevilEnemy: daca attackBox.size.x < 0, inseamna ca e un atac cu proiectil
                if (attackBox.size.x < 0) {
                    sf::Vector2f spawnPos = enemy->getPosition();
                    sf::Vector2f dir = player.getPosition() - spawnPos;
                    enemyProjectiles.push_back(std::make_unique<DevilProjectile>(spawnPos, dir));

                    activeSounds.emplace_back(m_soundBuffers["devil_attack"]);
                    activeSounds.back().setPitch(2.f);
                    activeSounds.back().play();
                }
            }
        }
    }

    // Gestioneaza mortile si aparitia monedelor
    for (auto& enemy : m_enemies) {
        if (enemy->hasJustDied()) {
            activeSounds.emplace_back(m_soundBuffers["coin_drop"]);
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

    // Sterge inamicii morti
    std::erase_if(m_enemies, [](const auto& enemy) {
        return enemy->isDead();
    });
}

void EnemyManager::draw(sf::RenderWindow& window) {
    for (auto& enemy : m_enemies) {
        enemy->draw(window);
    }
}

void EnemyManager::handleBulletCollisions(std::vector<Bullet>& bullets,
                                          std::list<sf::Sound>& activeSounds,
                                          std::vector<std::unique_ptr<Effect>>& effects,
                                          const sf::Texture& ghostImpactTexture,
                                          const std::vector<sf::IntRect>& ghostImpactFrames,
                                          const sf::Texture& bloodEffectTexture,
                                          const std::vector<sf::IntRect>& bloodEffectFrames) {
    for (auto& bullet : bullets) {
        if (!bullet.isImpacting()) {
            for (auto& enemy : m_enemies) {
                if (enemy->isDead()) continue;

                if (enemy->getBounds().findIntersection(bullet.getBounds())) {
                    float damage = bullet.getDamage();
                    enemy->takeDamage(damage);

                    if (dynamic_cast<ChaserEnemy*>(enemy.get())) {
                        activeSounds.emplace_back(m_soundBuffers["chaser_hurt"]);
                        activeSounds.back().setPitch(RandomGenerator::getFloat(0.9f, 1.1f));
                        activeSounds.back().setVolume(20.f);
                        activeSounds.back().play();
                    } else if (dynamic_cast<GhostEnemy*>(enemy.get())) {
                        activeSounds.emplace_back(m_soundBuffers["ghost_hurt"]);
                        activeSounds.back().setPitch(RandomGenerator::getFloat(1.1f, 1.3f));
                        activeSounds.back().setVolume(20.f);
                        activeSounds.back().play();
                    } else if (dynamic_cast<DevilEnemy*>(enemy.get())) {
                        activeSounds.emplace_back(m_soundBuffers["devil_hurt"]);
                        activeSounds.back().setPitch(RandomGenerator::getFloat(3.0f, 4.0f));
                        activeSounds.back().setVolume(20.f);
                        activeSounds.back().play();
                    }
                    bullet.hit();

                    if (dynamic_cast<GhostEnemy*>(enemy.get())) {
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

                    break; // Glontul a lovit un inamic nu mai verifica alti inamici pentru acest glont
                }
            }
        }
    }
}

void EnemyManager::spawnEnemy(const sf::FloatRect& mapBounds, const GameMap& gameMap) {
    sf::Vector2f randomPos;
    do {
        float x = RandomGenerator::getFloat(mapBounds.position.x, mapBounds.position.x + mapBounds.size.x);
        float y = RandomGenerator::getFloat(mapBounds.position.y, mapBounds.position.y + mapBounds.size.y);
        randomPos = {x, y};
    } while (gameMap.isSolid(randomPos));

    float randType = RandomGenerator::getFloat(0.f, 1.f);
    if (randType < 0.2f) {
        m_enemies.emplace_back(std::make_unique<ChaserEnemy>(m_soundBuffers));
    } else if (randType < 0.4f) {
        m_enemies.emplace_back(std::make_unique<GhostEnemy>(m_soundBuffers));
    } else {
        m_enemies.emplace_back(std::make_unique<DevilEnemy>(m_soundBuffers));
    }
    m_enemies.back()->setPosition(randomPos);
}

const std::vector<std::unique_ptr<EnemyBase>>& EnemyManager::getEnemies() const {
    return m_enemies;
}

std::vector<std::unique_ptr<EnemyBase>>& EnemyManager::getEnemies() {
    return m_enemies;
}
