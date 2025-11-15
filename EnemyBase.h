#pragma once
#ifndef __clangd__
#include <SFML/Graphics.hpp>
#endif
#include "GameMap.h"


class EnemyBase
{
public:
    virtual ~EnemyBase() = default;

    virtual void update(sf::Time dt, sf::Vector2f playerPosition,const GameMap &gameMap) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void setPosition(sf::Vector2f position) = 0;

    [[nodiscard]] virtual sf::Vector2f getPosition() const = 0;
    [[nodiscard]] virtual sf::FloatRect getBounds() const = 0;

    [[nodiscard]] virtual bool isAttacking() const = 0;
    virtual bool didAttackLand() = 0;
    [[nodiscard]] virtual sf::FloatRect getAttackHitbox() const = 0;

    virtual void takeDamage(float damage) = 0;
    [[nodiscard]] virtual bool isDead() const = 0;

    [[nodiscard]] virtual bool hasJustDied() const = 0;
    virtual void acknowledgeDeath() = 0;
    [[nodiscard]] virtual int getCoinValue() const = 0;

    [[nodiscard]] virtual std::unique_ptr<EnemyBase> clone() const = 0;
};