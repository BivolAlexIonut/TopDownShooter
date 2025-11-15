#pragma once
#include "EnemyBase.h"
#include "Health.h"
#include <map>
#include <vector>
#include <SFML/Audio.hpp>

class DevilEnemy : public EnemyBase {
public:
    enum class State {
        IDLE,
        FLYING,
        ATTACKING,
        HURT,
        DYING
    };

    explicit DevilEnemy(const std::map<std::string, sf::SoundBuffer>& soundBuffers);
    static bool initAssets(const std::string& basePath);

private:
    void updateAnimation();
    void updateHealthBar();
    static void setupAnimation(std::vector<sf::IntRect>& frames, int frameCount);

    sf::Sprite m_sprite;
    State m_currentState;
    State m_previousState;

    std::map<State, std::vector<sf::IntRect>> m_animations;
    static std::map<State, sf::Texture> s_textures;

    int m_currentFrame;
    float m_frameTime;
    sf::Clock m_animationTimer;
    sf::Clock m_hurtTimer;
    sf::Clock m_shootTimer;

    float m_speed;
    int m_damageFrame;
    bool m_didAttackLand;

    Health m_health;
    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBarForeground;

    bool m_isReadyForRemoval;
    bool m_justDied;

    sf::SoundBuffer soundBuffer;
    sf::Sound m_moveSound{soundBuffer};
    sf::Clock m_moveTimer;
    float m_moveCooldown;
    bool m_wasMoving;

protected:

    void doUpdate(sf::Time dt, sf::Vector2f playerPosition, const GameMap& gameMap) override;
    void doDraw(sf::RenderWindow& window) override;
    void doSetPosition(sf::Vector2f position) override;
    [[nodiscard]] sf::Vector2f doGetPosition() const override;
    [[nodiscard]] sf::FloatRect doGetBounds() const override;

    void doTakeDamage(float damage) override;
    [[nodiscard]] bool doIsDead() const override;
    [[nodiscard]] bool doIsAttacking() const override;
    bool doDidAttackLand() override;
    [[nodiscard]] sf::FloatRect doGetAttackHitbox() const override;

    [[nodiscard]] bool doHasJustDied() const override;
    void doAcknowledgeDeath() override;
    [[nodiscard]] std::unique_ptr<EnemyBase> doClone() const override;
    [[nodiscard]] int doGetCoinValue() const override;
};