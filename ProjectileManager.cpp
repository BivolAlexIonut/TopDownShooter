#include "ProjectileManager.h"
#include "RandomGenerator.h"
#include <cmath>
#include <algorithm>

void ProjectileManager::update(float dt, const GameMap& map, Player& player, sf::Clock& damageTimer, float iframeDuration, std::list<sf::Sound>& activeSounds, sf::SoundBuffer& hurtSound) {
    // Update gloante jucator
    for (auto& bullet : m_bullets) {
        bullet.update(dt);
        if (!bullet.isImpacting()) {
            sf::Vector2f pos = bullet.getPosition();
            sf::Vector2f vel = bullet.getVelocity();
            float length = std::sqrt(vel.x * vel.x + vel.y * vel.y);
            sf::Vector2f normVel = (length != 0.f) ? vel / length : sf::Vector2f(0.f, 0.f);
            
            if (map.isSolid(pos + normVel * 25.f)) {
                bullet.hit();
            }
        }
    }

    // Update proiectile inamici
    for (auto& proj : m_enemyProjectiles) {
        proj->update(dt, map);
        if (proj->getBounds().findIntersection(player.getCollisionBounds()) &&
            damageTimer.getElapsedTime().asSeconds() > iframeDuration) {
            
            sf::Vector2f knockbackDir = player.getPosition() - proj->getBounds().position;
            float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
            knockbackDir = (length != 0.f) ? knockbackDir / length : sf::Vector2f(1.f, 0.f);

            player.takeDamage(15.f, knockbackDir);
            activeSounds.emplace_back(hurtSound);
            activeSounds.back().setPitch(RandomGenerator::getFloat(0.5f, 1.2f));
            activeSounds.back().setVolume(80.f);
            activeSounds.back().play();
            damageTimer.restart();
            proj->hit();
        }
    }

    // Curatare
    m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(), [](const Bullet& b) { return b.isDead(); }), m_bullets.end());
    std::erase_if(m_enemyProjectiles, [](const auto& proj) { return proj->isDead(); });
}

void ProjectileManager::draw(sf::RenderWindow& window) {
    for (const auto& proj : m_enemyProjectiles) proj->draw(window);
    for (const auto& bullet : m_bullets) bullet.draw(window);
}

void ProjectileManager::clear() {
    m_bullets.clear();
    m_enemyProjectiles.clear();
}

void ProjectileManager::addBullet(const Bullet& bullet) {
    m_bullets.push_back(bullet);
}

void ProjectileManager::addEnemyProjectile(std::unique_ptr<DevilProjectile> projectile) {
    m_enemyProjectiles.push_back(std::move(projectile));
}

std::vector<Bullet>& ProjectileManager::getBullets() {
    return m_bullets;
}

std::vector<std::unique_ptr<DevilProjectile>>& ProjectileManager::getEnemyProjectiles() {
    return m_enemyProjectiles;
}
