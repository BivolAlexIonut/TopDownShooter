#pragma once
#include <SFML/Graphics.hpp>
#include "GameMap.h"

class DevilProjectile {
public:
    DevilProjectile(sf::Vector2f startPos,sf::Vector2f direction);
    ~DevilProjectile() = default;

    static bool initAssets(const std::string& texturePath);

    void update(float dt,const GameMap& gameMap);

    void draw(sf::RenderWindow& window) const;

    [[nodiscard]] sf::FloatRect getBounds() const;
    [[nodiscard]] bool isDead() const;
    void hit();

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
    bool m_isDead;

    static sf::Texture s_texture;
};