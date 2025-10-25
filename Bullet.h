// Bullet.h
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Bullet {
public:
    Bullet(sf::Texture& texture, const std::vector<sf::IntRect>& animRects, sf::Vector2f startPos, sf::Vector2f direction,
           float animSpeed);
    ~Bullet();
    void draw(sf::RenderWindow& window);
    void update(float dt);

private:
    sf::Sprite bulletSprite;
    sf::Vector2f bulletVelocity;

    sf::IntRect bulletRect;
    sf::Clock bulletTimer;
    std::vector<sf::IntRect> m_animRects;

    int bulletCurrentFrame;
    float bulletAnimSpeed;
    int m_animFrames;
};