#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Effect {
public:
    Effect(sf::Texture& texture,
           const std::vector<sf::IntRect>& animRects,
           sf::Vector2f position,
           float animSpeed,
           sf::Vector2f scale,sf::Color = sf::Color::White);

    void update();
    void draw(sf::RenderWindow& window) const;
    [[nodiscard]] bool isDead() const;

private:
    sf::Sprite m_sprite;
    std::vector<sf::IntRect> m_animRects;
    int m_currentFrame;
    float m_animSpeed;
    sf::Clock m_animTimer;
    bool m_isDead;
};