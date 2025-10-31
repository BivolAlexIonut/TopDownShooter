#pragma once
#include <SFML/Graphics.hpp>



class EnemyBase
{
public:
    virtual ~EnemyBase() = default;

    virtual void update(sf::Time dt, sf::Vector2f playerPosition) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void setPosition(sf::Vector2f position) = 0;

    [[nodiscard]] virtual sf::Vector2f getPosition() const = 0;
    [[nodiscard]] virtual sf::FloatRect getBounds() const = 0;

    [[nodiscard]] virtual bool isAttacking() const = 0;
    virtual bool didAttackLand() = 0;
    [[nodiscard]] virtual sf::FloatRect getAttackHitbox() const = 0;
};