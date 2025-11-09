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

    ChaserEnemy();

    static bool initAssets();

    void update(sf::Time dt, sf::Vector2f playerPosition,const GameMap &gameMap) override;
    void draw(sf::RenderWindow& window) override;
    void setPosition(sf::Vector2f position) override;
    sf::Vector2f getPosition() const override;
    sf::FloatRect getBounds() const override;

    //Inamic
    void takeDamage(float damage) override;
    bool isDead() const override;

    [[nodiscard]] bool hasJustDied() const override;
    void acknowledgeDeath() override;

    bool isAttacking() const override;
    bool didAttackLand() override;
    sf::FloatRect getAttackHitbox() const override;

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
};