#pragma once
#include <map>
#include <vector>
#include "EnemyBase.h"
#include "Player.h"

class ChaserEnemy : public EnemyBase
{
public:
    enum class State {
        IDLE,
        MOVING,
        ATTACKING
    };

    ChaserEnemy();

    void update(sf::Time dt, sf::Vector2f playerPosition) override;
    void draw(sf::RenderWindow& window) override;
    void setPosition(sf::Vector2f position) override;
    sf::Vector2f getPosition() const override;
    sf::FloatRect getBounds() const override;

    bool isAttacking() const override;
    bool didAttackLand() override;
    sf::FloatRect getAttackHitbox() const override;

private:
    float m_speed;

    void updateAnimation();

    sf::Sprite m_sprite;
    sf::Texture m_texture;
    State m_currentState;

    std::map<State,std::vector<sf::IntRect>> m_animations;

    int m_currentFrame;
    float m_frameTime;
    sf::Clock m_animationTimer;

    bool m_didAttackLand;
    int m_damageFrame;
    float m_currentAngleRad;
};