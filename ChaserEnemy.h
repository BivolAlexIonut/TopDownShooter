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
        ATTACKING,
        ALERTED,
        DYING
    };

    explicit ChaserEnemy(const std::map<std::string, sf::SoundBuffer>& soundBuffers);
    static bool initAssets();

private:
    float m_speed;

    void updateAnimation();
    void updateHealthBar();

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

    State m_previousState;

    sf::Sprite m_alertSprite;
    sf::Clock m_alertTimer;
    float m_alertDuration;

    bool m_isReadyForRemoval;
    bool m_justDied;

    static sf::Texture s_texture;
    static sf::Texture s_alertTexture;
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
    sf::Vector2f doGetPosition() const override;
    sf::FloatRect doGetBounds() const override;
    void doTakeDamage(float damage) override;
    bool doIsDead() const override;
    [[nodiscard]] bool doHasJustDied() const override;
    void doAcknowledgeDeath() override;
    [[nodiscard]] int doGetCoinValue() const override;
    [[maybe_unused]] [[nodiscard]] std::unique_ptr<EnemyBase> doClone() const override;
    [[maybe_unused]] bool doIsAttacking() const override;
    bool doDidAttackLand() override;
    sf::FloatRect doGetAttackHitbox() const override;
};