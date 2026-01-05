#pragma once
#include <vector>
#include <memory>
#include <list>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "EnemyBase.h"
#include "Player.h"
#include "GameMap.h"
#include "Bullet.h"
#include "EffectManager.h"
#include "Coin.h"
#include "DevilProjectile.h"

class EnemyManager {
public:
    explicit EnemyManager(std::map<std::string, sf::SoundBuffer>& soundBuffers);

    void reset();
    void update(float dt, Player& player, const GameMap& map,
                std::list<sf::Sound>& activeSounds,
                std::vector<std::unique_ptr<Coin>>& coins,
                std::vector<std::unique_ptr<DevilProjectile>>& enemyProjectiles,
                const sf::FloatRect& mapBounds,
                sf::Clock& playerDamageTimer,
                float playerIframeDuration);

    void draw(sf::RenderWindow& window);

    void handleBulletCollisions(std::vector<Bullet>& bullets,
                                std::list<sf::Sound>& activeSounds,
                                EffectManager& effectManager,
                                const sf::Texture& ghostImpactTexture,
                                const std::vector<sf::IntRect>& ghostImpactFrames,
                                const sf::Texture& bloodEffectTexture,
                                const std::vector<sf::IntRect>& bloodEffectFrames);

private:
    void spawnEnemy(const sf::FloatRect& mapBounds, const GameMap& gameMap);

    std::vector<std::unique_ptr<EnemyBase>> m_enemies;
    std::map<std::string, sf::SoundBuffer>& m_soundBuffers;

    sf::Clock m_respawnTimer;
    const float m_respawnDelay = 6.0f;
    const size_t m_maxEnemies = 10;
};