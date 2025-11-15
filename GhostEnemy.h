#pragma once
#include "EnemyBase.h"
#include "Health.h"
#include <map>
#include <vector>
#include <SFML/Audio.hpp>

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

    explicit GhostEnemy(const std::map<std::string, sf::SoundBuffer>& soundBuffers);

    static bool initAssets();

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
    bool m_justDied;

    sf::Clock m_abilityTimer;
    float m_abilityChargeTime;

    static std::map<State, sf::Texture> s_stateTextures;
    static sf::Texture s_deathTexture;

    sf::SoundBuffer soundBuffer;
    sf::Sound m_moveSound{soundBuffer};
    sf::Clock m_moveTimer;
    float m_moveCooldown;
    bool m_wasMoving;

protected:

    void doUpdate(sf::Time dt, sf::Vector2f playerPosition, const GameMap &gameMap) override;
    void doDraw(sf::RenderWindow& window) override;
    void doSetPosition(sf::Vector2f position) override;
    [[nodiscard]] sf::Vector2f doGetPosition() const override;
    [[nodiscard]] sf::FloatRect doGetBounds() const override;
    [[nodiscard]] bool doIsAttacking() const override;
    bool doDidAttackLand() override;
    [[nodiscard]] sf::FloatRect doGetAttackHitbox() const override;
    void doTakeDamage(float damage) override;
    [[nodiscard]] bool doIsDead() const override;
    [[nodiscard]] bool doHasJustDied() const override;
    void doAcknowledgeDeath() override;
    [[nodiscard]] int doGetCoinValue() const override;
    [[nodiscard]] std::unique_ptr<EnemyBase> doClone() const override;
};