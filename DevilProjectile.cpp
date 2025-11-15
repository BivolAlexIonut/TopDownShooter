#include "DevilProjectile.h"
#include "GameExceptions.h"
#include <cmath>

sf::Texture DevilProjectile::s_texture;

bool DevilProjectile::initAssets(const std::string& texturePath)
{
    if (!s_texture.loadFromFile(texturePath)) {
        throw AssetLoadException(texturePath);
    }
    return true;
}

DevilProjectile::DevilProjectile(sf::Vector2f startPos, sf::Vector2f direction) : m_sprite(s_texture),m_isDead(false) {

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    m_sprite.setPosition(startPos);

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0.f) {
        direction /= length;
    }
    float speed = 1200.f;
    m_velocity = direction * speed;

    float angleRad = std::atan2(direction.y, direction.x);
    sf::Angle rotation = sf::radians(angleRad);
    m_sprite.setRotation(rotation + sf::degrees(180.f));
}

void DevilProjectile::update(float dt, const GameMap& gameMap)
{
    if (m_isDead) return;

    m_sprite.move(m_velocity * dt);

    if (gameMap.isSolid(m_sprite.getPosition())) {
        m_isDead = true;
    }
}

void DevilProjectile::draw(sf::RenderWindow& window) const
{
    if (!m_isDead) {
        window.draw(m_sprite);
    }
}

sf::FloatRect DevilProjectile::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

bool DevilProjectile::isDead() const
{
    return m_isDead;
}

void DevilProjectile::hit()
{
    m_isDead = true;
}