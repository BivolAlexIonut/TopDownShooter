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

    void update(sf::Time dt, sf::Vector2f playerPosition, const GameMap& gameMap) override;
    void draw(sf::RenderWindow& window) override;
    void setPosition(sf::Vector2f position) override;
    [[nodiscard]] sf::Vector2f getPosition() const override;
    [[nodiscard]] sf::FloatRect getBounds() const override;

    void takeDamage(float damage) override;
    [[nodiscard]] bool isDead() const override;
    [[nodiscard]] bool isAttacking() const override;
    bool didAttackLand() override;
    [[nodiscard]] sf::FloatRect getAttackHitbox() const override;

    [[nodiscard]] bool hasJustDied() const override;
    void acknowledgeDeath() override;
    [[nodiscard]] std::unique_ptr<EnemyBase> clone() const override;
    [[nodiscard]] int getCoinValue() const override;

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
};