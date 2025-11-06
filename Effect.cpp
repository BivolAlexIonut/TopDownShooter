#include "Effect.h"

Effect::Effect(const sf::Texture& texture, const std::vector<sf::IntRect>& animRects, sf::Vector2f position, float animSpeed,
    sf::Vector2f scale)
    : m_sprite(texture),
      m_animRects(animRects),
      m_currentFrame(0),
      m_animSpeed(animSpeed),
      m_isDead(false)
{
    m_sprite.setTextureRect(m_animRects[0]);

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});

    m_sprite.setPosition(position);
    m_sprite.setScale(scale);
}

void Effect::update() {
    if (m_isDead) return;
    if (m_animTimer.getElapsedTime().asSeconds() > m_animSpeed) {
        m_currentFrame++;
        m_animTimer.restart();
    }

    if (static_cast<size_t>(m_currentFrame) >= m_animRects.size()) {
        m_isDead = true;
    } else {
        m_sprite.setTextureRect(m_animRects[m_currentFrame]);
    }
}

void Effect::draw(sf::RenderWindow& window) const {
    if (!m_isDead) {
        window.draw(m_sprite);
    }
}

bool Effect::isDead() const {
    return m_isDead;
}