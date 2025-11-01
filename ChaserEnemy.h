#pragma once
#include <map>
#include <vector>
#include "EnemyBase.h"
#include "Player.h"
#include "Health.h"

class ChaserEnemy : public EnemyBase
{
public:
    enum class State {
        IDLE,
        MOVING,
        ATTACKING
    };

    ChaserEnemy();

    void update(sf::Time dt, sf::Vector2f playerPosition,const GameMap &gameMap) override;
    void draw(sf::RenderWindow& window) override;
    void setPosition(sf::Vector2f position) override;
    sf::Vector2f getPosition() const override;
    sf::FloatRect getBounds() const override;

    //Inamic
    void takeDamage(float damage);
    bool isDead() const;

    bool isAttacking() const override;
    bool didAttackLand() override;
    sf::FloatRect getAttackHitbox() const override;

private:
    float m_speed;

    void updateAnimation();
    void updateHealthBar();

    sf::Texture m_texture;
    sf::Sprite m_sprite;
    State m_currentState;

    std::map<State,std::vector<sf::IntRect>> m_animations;

    int m_currentFrame;
    float m_frameTime;
    sf::Clock m_animationTimer;

    bool m_didAttackLand;
    int m_damageFrame;
    float m_currentAngleRad;

    Health m_health;
    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBarForeground;
};