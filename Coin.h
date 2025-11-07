#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Coin {
public:
    explicit Coin(sf::Vector2f startPos);

    static bool initAssets();

    void update();
    void draw(sf::RenderWindow &window) const;
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

private:
    sf::Sprite m_sprite;
    int m_currentFrame;
    float m_animSpeed;
    sf::Clock m_animTimer;
    std::vector<sf::IntRect> m_animRects;

    static sf::Texture s_texture;
};