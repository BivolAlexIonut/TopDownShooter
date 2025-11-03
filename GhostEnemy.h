#pragma once
#include "EnemyBase.h"
#include "Health.h"
#include <map>
#include <vector>

class GhostEnemy : public EnemyBase
{
public:
    enum class AttackDirection {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    enum class State {
        IDLE,
        MOVING,
        ATTACKING,
        ABILITY,
        DYING
    };

    GhostEnemy();

    static bool initAssets();

    void update(sf::Time dt, sf::Vector2f playerPosition, const GameMap& gameMap) override;
    void draw(sf::RenderWindow& window) override;
    void setPosition(sf::Vector2f position) override;
    sf::Vector2f getPosition() const override;
    sf::FloatRect getBounds() const override;
    bool isAttacking() const override;
    bool didAttackLand() override;
    sf::FloatRect getAttackHitbox() const override;
    void takeDamage(float damage) override;
    bool isDead() const override;

private:
    void updateAnimation();
    void updateHealthBar();

    sf::Sprite m_sprite;
    State m_currentState;
    State m_previousState;

    std::map<State, std::vector<sf::IntRect>> m_animations;
    static std::map<AttackDirection, std::vector<sf::IntRect>> s_attackAnimations;
    AttackDirection m_lockedAttackDirection;

    int m_currentFrame;
    float m_frameTime;
    sf::Clock m_animationTimer;

    float m_speed;
    int m_damageFrame;
    float m_currentAngleRad;
    bool m_didAttackLand;

    Health m_health;
    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBarForeground;
    bool m_isReadyForRemoval;

    sf::Clock m_abilityTimer;
    float m_abilityChargeTime;

    static std::map<State, sf::Texture> s_stateTextures;
    static sf::Texture s_deathTexture;
};