#pragma once
#include <SFML/Graphics.hpp>
#include "GameMap.h"
#include <memory>

class EnemyBase
{
public:
    virtual ~EnemyBase() = default;

    void update(sf::Time dt, sf::Vector2f playerPosition, const GameMap &gameMap)
    {
        if (isDead()) return;
        doUpdate(dt, playerPosition, gameMap);
    }

    void draw(sf::RenderWindow& window)
    {
        doDraw(window);
    }

    void setPosition(sf::Vector2f position)
    {
        doSetPosition(position);
    }

    [[nodiscard]] sf::Vector2f getPosition() const
    {
        return doGetPosition();
    }
    [[nodiscard]] sf::FloatRect getBounds() const
    {
        if (isDead()) return {};
        return doGetBounds();
    }

    [[nodiscard]] bool isAttacking() const
    {
        return doIsAttacking();
    }

    bool didAttackLand()
    {
        return doDidAttackLand();
    }

    [[nodiscard]] sf::FloatRect getAttackHitbox() const
    {
        return doGetAttackHitbox();
    }

    void takeDamage(float damage)
    {
        if (isDead()) return;
        doTakeDamage(damage);
    }

    [[nodiscard]] bool isDead() const
    {
        return doIsDead();
    }

    [[nodiscard]] bool hasJustDied() const
    {
        return doHasJustDied();
    }

    void acknowledgeDeath()
    {
        doAcknowledgeDeath();
    }

    [[nodiscard]] std::unique_ptr<EnemyBase> clone() const
    {
        return doClone();
    }

    [[nodiscard]] int getCoinValue() const
    {
        return doGetCoinValue();
    }

protected:

    virtual void doUpdate(sf::Time dt, sf::Vector2f playerPosition, const GameMap &gameMap) = 0;
    virtual void doDraw(sf::RenderWindow& window) = 0;
    virtual void doSetPosition(sf::Vector2f position) = 0;

    [[nodiscard]] virtual sf::Vector2f doGetPosition() const = 0;
    [[nodiscard]] virtual sf::FloatRect doGetBounds() const = 0;

    [[nodiscard]] virtual bool doIsAttacking() const = 0;
    virtual bool doDidAttackLand() = 0;
    [[nodiscard]] virtual sf::FloatRect doGetAttackHitbox() const = 0;

    virtual void doTakeDamage(float damage) = 0;
    [[nodiscard]] virtual bool doIsDead() const = 0;

    [[nodiscard]] virtual bool doHasJustDied() const = 0;
    virtual void doAcknowledgeDeath() = 0;
    [[nodiscard]] virtual std::unique_ptr<EnemyBase> doClone() const = 0;
    [[nodiscard]] virtual int doGetCoinValue() const = 0;
};