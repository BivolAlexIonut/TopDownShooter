#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Bullet {
public:
    Bullet(sf::Texture &texture, const std::vector<sf::IntRect> &animRects, sf::Vector2f startPos,
           sf::Vector2f direction,
           float animSpeed, sf::Vector2f scale);

    ~Bullet();

    void draw(sf::RenderWindow &window);

    void update(float dt);

    sf::Vector2f getPosition() const;

    sf::Vector2f getVelocity() const;

    void hit();

    [[nodiscard]] bool isDead() const;

    [[nodiscard]] bool isImpacting() const;

    friend std::ostream &operator<<(std::ostream &os, const Bullet &b);

private:
    sf::Sprite bulletSprite;
    sf::Vector2f bulletVelocity;

    sf::IntRect bulletRect;
    sf::Clock bulletTimer;
    std::vector<sf::IntRect> m_animRects;

    int bulletCurrentFrame;
    float bulletAnimSpeed;
    int m_animFrames;

    bool m_isDead;
    bool m_isImpacting;
};
