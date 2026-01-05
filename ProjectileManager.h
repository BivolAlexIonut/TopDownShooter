#pragma once
#include <vector>
#include <memory>
#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Bullet.h"
#include "DevilProjectile.h"
#include "GameMap.h"
#include "Player.h"

class ProjectileManager {
public:
    void update(float dt, const GameMap& map, Player& player, sf::Clock& damageTimer, float iframeDuration, std::list<sf::Sound>& activeSounds, sf::SoundBuffer& hurtSound);
    void draw(sf::RenderWindow& window);
    void clear();

    void addBullet(const Bullet& bullet);
    void addEnemyProjectile(std::unique_ptr<DevilProjectile> projectile);

    std::vector<Bullet>& getBullets();
    std::vector<std::unique_ptr<DevilProjectile>>& getEnemyProjectiles();

private:
    std::vector<Bullet> m_bullets;
    std::vector<std::unique_ptr<DevilProjectile>> m_enemyProjectiles;
};