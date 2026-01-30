#include "PickableManager.h"
#include "RandomGenerator.h"
#include <cmath>

void PickableManager::update(float dt, Player& player, std::list<sf::Sound>& activeSounds, sf::SoundBuffer& pickupSound) {
    sf::Vector2f playerPos = player.getPosition();
    float radiusSq = m_magnetRadius * m_magnetRadius;

    for (auto& coin : m_coins) {
        coin->update();
        sf::Vector2f coinPos = coin->getPosition();
        sf::Vector2f dir = playerPos - coinPos;
        float distSq = dir.x * dir.x + dir.y * dir.y;

        if (distSq < radiusSq && distSq > 0.01f) {
            float dist = std::sqrt(distSq);
            coin->setPosition(coinPos + (dir / dist) * m_magnetSpeed * dt);
        }
    }

    std::erase_if(m_coins, [&](const auto& coin) {
        if (player.getCollisionBounds().findIntersection(coin->getBounds())) {
            activeSounds.emplace_back(pickupSound);
            activeSounds.back().setPitch(RandomGenerator::get<float>(1.1f, 1.3f));
            activeSounds.back().setVolume(100.f);
            activeSounds.back().play();
            player.addCoins(1);
            return true;
        }
        return coin->isDead();
    });
}

void PickableManager::draw(sf::RenderWindow& window) const {
    for (const auto& coin : m_coins) coin->draw(window);
}

void PickableManager::clear() {
    m_coins.clear();
}

void PickableManager::addCoin(std::unique_ptr<Coin> coin) {
    m_coins.push_back(std::move(coin));
}
