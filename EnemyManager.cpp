#include "EnemyManager.h"
#include "ChaserEnemy.h"
#include "GhostEnemy.h"
#include "DevilEnemy.h"
#include "RandomGenerator.h"
#include <iostream>
#include <algorithm>
#include <cmath>

EnemyManager::EnemyManager()
{
}

void EnemyManager::reset() {
    m_enemies.clear();
    m_respawnTimer.restart();
}

void EnemyManager::update(float dt, Player& player, const GameMap& map,
                          std::list<sf::Sound>& activeSounds,
                          std::vector<std::unique_ptr<Coin>>& coins,
                          std::vector<std::unique_ptr<DevilProjectile>>& enemyProjectiles,
                          const sf::FloatRect& mapBounds,
                          sf::Clock& playerDamageTimer,
                          float playerIframeDuration) {

    if (m_respawnTimer.getElapsedTime().asSeconds() > m_respawnDelay) {
        if (m_enemies.size() < m_maxEnemies) {
            int enemiesToSpawn = static_cast<int>(m_maxEnemies - m_enemies.size());
            for (int i = 0; i < enemiesToSpawn; ++i) {
                spawnEnemy(mapBounds, map);
            }
        }
        m_respawnTimer.restart();
    }

    for (auto& enemy : m_enemies) {
        if (!enemy->isDead()) {
            enemy->update(sf::seconds(dt), player.getPosition(), map);

            if (enemy->didAttackLand()) {
                sf::FloatRect attackBox = enemy->getAttackHitbox();
                
                if (attackBox.size.x < 0) {
                    sf::Vector2f spawnPos = enemy->getPosition();
                    sf::Vector2f dir = player.getPosition() - spawnPos;
                    enemyProjectiles.push_back(std::make_unique<DevilProjectile>(spawnPos, dir));

                    activeSounds.emplace_back(ResourceManager::getInstance().getSoundBuffer("devil_attack"));
                    activeSounds.back().setPitch(2.f);
                    activeSounds.back().play();
                } 
                else {
                    if (attackBox.findIntersection(player.getCollisionBounds()) &&
                        playerDamageTimer.getElapsedTime().asSeconds() > playerIframeDuration) {
                        
                        sf::Vector2f knockbackDir = player.getPosition() - enemy->getPosition();
                        float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
                        if (length != 0.f) knockbackDir /= length; else knockbackDir = {1.f, 0.f};

                        player.takeDamage(10.f, knockbackDir);
                        activeSounds.emplace_back(ResourceManager::getInstance().getSoundBuffer("player_hurt"));
                        activeSounds.back().setPitch(RandomGenerator::get<float>(0.5f, 1.2f));
                        activeSounds.back().setVolume(80.f);
                        activeSounds.back().play();
                        playerDamageTimer.restart();

                        if (dynamic_cast<ChaserEnemy *>(enemy.get())) {
                            activeSounds.emplace_back(ResourceManager::getInstance().getSoundBuffer("chaser_attack"));
                            activeSounds.back().setVolume(20.f);
                            activeSounds.back().play();
                        } else if (dynamic_cast<GhostEnemy *>(enemy.get())) {
                            activeSounds.emplace_back(ResourceManager::getInstance().getSoundBuffer("ghost_attack"));
                            activeSounds.back().setVolume(20.f);
                            activeSounds.back().play();
                        }
                    }
                }
            }
        }
    }

    for (auto& enemy : m_enemies) {
        if (enemy->hasJustDied()) {
            activeSounds.emplace_back(ResourceManager::getInstance().getSoundBuffer("coin_drop"));
            activeSounds.back().setPitch(RandomGenerator::get<float>(0.9f, 1.1f));
            activeSounds.back().setVolume(100.f);
            activeSounds.back().play();

            int coinCount = enemy->getCoinValue();
            for (int c = 0; c < coinCount; ++c) {
                sf::Vector2f pos = enemy->getPosition();
                pos.x += RandomGenerator::get<float>(-40.f, 40.f);
                pos.y += RandomGenerator::get<float>(-40.f, 40.f);
                coins.push_back(std::make_unique<Coin>(pos));
            }
            enemy->acknowledgeDeath();
        }
    }

    std::erase_if(m_enemies, [](const auto& enemy) { return enemy->isDead(); });
}

void EnemyManager::draw(sf::RenderWindow& window) {
    for (auto& enemy : m_enemies) enemy->draw(window);
}

void EnemyManager::handleBulletCollisions(std::vector<Bullet>& bullets,
                                          std::list<sf::Sound>& activeSounds,
                                          EffectManager& effectManager,
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

                    std::string hurtKey;
                    if (dynamic_cast<ChaserEnemy*>(enemy.get())) hurtKey = "chaser_hurt";
                    else if (dynamic_cast<GhostEnemy*>(enemy.get())) hurtKey = "ghost_hurt";
                    else if (dynamic_cast<DevilEnemy*>(enemy.get())) hurtKey = "devil_hurt";

                    if (!hurtKey.empty()) {
                        activeSounds.emplace_back(ResourceManager::getInstance().getSoundBuffer(hurtKey));
                        activeSounds.back().setPitch(RandomGenerator::get<float>(0.9f, 1.1f));
                        activeSounds.back().setVolume(20.f);
                        activeSounds.back().play();
                    }
                    bullet.hit();

                    if (dynamic_cast<GhostEnemy*>(enemy.get())) {
                        effectManager.addEffect(std::make_unique<Effect>(ghostImpactTexture, ghostImpactFrames, bullet.getPosition(), 0.03f, sf::Vector2f(0.6f, 0.6f)));
                    } else {
                        float scale = std::clamp(0.25f + (damage / 200.f), 0.2f, 2.0f);
                        effectManager.addEffect(std::make_unique<Effect>(bloodEffectTexture, bloodEffectFrames, bullet.getPosition(), 0.01f, sf::Vector2f(scale, scale)));
                    }
                    break;
                }
            }
        }
    }
}

void EnemyManager::spawnEnemy(const sf::FloatRect& mapBounds, const GameMap& gameMap) {
    sf::Vector2f pos;
    do {
        pos = {RandomGenerator::get<float>(mapBounds.position.x, mapBounds.position.x + mapBounds.size.x),
               RandomGenerator::get<float>(mapBounds.position.y, mapBounds.position.y + mapBounds.size.y)};
    } while (gameMap.isSolid(pos));

    int rand = RandomGenerator::get<int>(0, 100);
    if (rand < 20) m_enemies.emplace_back(EnemyFactory::createEnemy(EnemyType::Chaser));
    else if (rand < 40) m_enemies.emplace_back(EnemyFactory::createEnemy(EnemyType::Ghost));
    else m_enemies.emplace_back(EnemyFactory::createEnemy(EnemyType::Devil));
    
    if (!m_enemies.empty() && m_enemies.back()) {
        m_enemies.back()->setPosition(pos);
    }
}