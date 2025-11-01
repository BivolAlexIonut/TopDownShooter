#include "ChaserEnemy.h"
#include <cmath>
#include <stdexcept>

ChaserEnemy::ChaserEnemy()
    : m_speed(270.f),
      m_sprite(this->m_texture),
      m_currentState(State::IDLE),
      m_currentFrame(0), m_frameTime(0.05f),m_didAttackLand(false),m_damageFrame(8),m_currentAngleRad(0.f),
      m_health(100.f,100.f){

    if (!m_texture.loadFromFile("assets/enemies/chaser_spritesheet_bun_final.png")) {
        throw std::runtime_error("EROARE: Nu am putut incarca chaser_spritesheet.png!");
    }
    m_sprite.setTexture(m_texture);

    constexpr int FRAME_WIDTH = 288;
    constexpr int FRAME_HEIGHT = 311;
    const float HEALTH_BAR_WIDTH = 50.f;
    const float HEALTH_BAR_HEIGHT = 5.f;

    m_healthBarBackground.setSize({HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT});
    m_healthBarBackground.setFillColor(sf::Color(100, 100, 100, 150));
    m_healthBarBackground.setOrigin({HEALTH_BAR_WIDTH / 2.f, HEALTH_BAR_HEIGHT / 2.f});

    m_healthBarForeground.setSize({HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT});
    m_healthBarForeground.setFillColor(sf::Color(255, 0, 0, 200));
    m_healthBarForeground.setOrigin({HEALTH_BAR_WIDTH / 2.f, HEALTH_BAR_HEIGHT / 2.f});

    std::vector<sf::IntRect> movingFrames;
    for (int i = 0; i < 16; ++i) {
        constexpr int TOP_Y_MOVE = 0;
        int left_x = i * FRAME_WIDTH;
        movingFrames.push_back(sf::IntRect({left_x, TOP_Y_MOVE}, {FRAME_WIDTH, FRAME_HEIGHT}));
    }

    std::vector<sf::IntRect> idleFrames;
    constexpr int TOP_Y_IDLE = FRAME_HEIGHT;
    for (int i = 0; i < 16; ++i) {
        int left_x = i * FRAME_WIDTH;
        idleFrames.push_back(sf::IntRect({left_x, TOP_Y_IDLE}, {FRAME_WIDTH, FRAME_HEIGHT}));
    }

    std::vector<sf::IntRect> attackingFrames;
    constexpr int TOP_Y_ATTACK = FRAME_HEIGHT * 2;
    for (int i = 0; i < 9; ++i) {
        int left_x = i * FRAME_WIDTH;
        attackingFrames.push_back(sf::IntRect({left_x, TOP_Y_ATTACK}, {FRAME_WIDTH, FRAME_HEIGHT}));
    }

    m_animations[State::MOVING] = movingFrames;
    m_animations[State::IDLE] = idleFrames;
    m_animations[State::ATTACKING] = attackingFrames;

    m_sprite.setTextureRect(m_animations[State::IDLE][0]);
    m_sprite.setOrigin({static_cast<float>(FRAME_WIDTH) / 2.f, static_cast<float>(FRAME_HEIGHT) / 2.f});
    float scaleValue = 0.45f;
    m_sprite.setScale({scaleValue, scaleValue});
}


void ChaserEnemy::update(const sf::Time dt, sf::Vector2f playerPosition, const GameMap& gameMap)
{
    sf::Vector2f currentPosition = m_sprite.getPosition();
    sf::Vector2f direction = playerPosition - currentPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    const float attackRange = 60.f;
    const float moveRange = 500.f;

    if (m_currentState != State::ATTACKING)
    {
        if (length < attackRange) {
            m_currentState = State::ATTACKING;
            m_currentFrame = 0;
            m_animationTimer.restart();
        } else if (length < moveRange) {
            m_currentState = State::MOVING;
        } else {
            m_currentState = State::IDLE;
        }
    }

    sf::Vector2f velocity(0.f, 0.f);

    if (m_currentState == State::MOVING)
    {
        sf::Vector2f normalizedDirection(0.f, 0.f);
        if (length != 0)
        {
            normalizedDirection = direction / length;
        }
        velocity = normalizedDirection * m_speed;
    }

    sf::Vector2f frameVelocity = velocity * dt.asSeconds();
    sf::FloatRect bounds = getBounds();

    bounds.position.x += frameVelocity.x;
    float topY = bounds.position.y;
    float bottomY = bounds.position.y + bounds.size.y;

    if (velocity.x > 0) {
        float rightX = bounds.position.x + bounds.size.x;
        if (gameMap.isSolid({rightX, topY}) || gameMap.isSolid({rightX, bottomY})) {
            frameVelocity.x = 0;
        }
    } else if (velocity.x < 0) {
        float leftX = bounds.position.x;
        if (gameMap.isSolid({leftX, topY}) || gameMap.isSolid({leftX, bottomY})) {
            frameVelocity.x = 0;
        }
    }
    m_sprite.move({frameVelocity.x, 0.f});

    bounds = getBounds();
    bounds.position.y += frameVelocity.y;
    float leftX = bounds.position.x;
    float rightX = bounds.position.x + bounds.size.x;

    if (velocity.y > 0) {
        float bottomY_check = bounds.position.y + bounds.size.y;
        if (gameMap.isSolid({leftX, bottomY_check}) || gameMap.isSolid({rightX, bottomY_check})) {
            frameVelocity.y = 0;
        }
    } else if (velocity.y < 0) {
        float topY_check = bounds.position.y;
        if (gameMap.isSolid({leftX, topY_check}) || gameMap.isSolid({rightX, topY_check})) {
            frameVelocity.y = 0;
        }
    }
    m_sprite.move({0.f, frameVelocity.y});
    sf::Vector2f dir = playerPosition - getPosition();
    m_currentAngleRad = std::atan2(dir.y, dir.x);
    float angleInDegrees = m_currentAngleRad * (180.f / 3.14159265f);
    m_sprite.setRotation(sf::degrees(angleInDegrees));

    updateAnimation();
    updateHealthBar();
}


void ChaserEnemy::updateHealthBar()
{
    float healthPercent = m_health.getPercentage();
    float barWidth = m_healthBarBackground.getSize().x;
    m_healthBarForeground.setSize({barWidth * healthPercent, m_healthBarForeground.getSize().y});

    sf::FloatRect spriteBounds = m_sprite.getGlobalBounds();

    float healthBarOffsetY = (spriteBounds.size.y / 2.f) + 10.f;

    sf::Vector2f barPos = {
        m_sprite.getPosition().x,
        m_sprite.getPosition().y - healthBarOffsetY
    };

    m_healthBarBackground.setPosition(barPos);
    m_healthBarForeground.setPosition(barPos);
}

sf::FloatRect ChaserEnemy::getBounds() const {
    const float boxWidth = 30.f;
    const float boxHeight = 30.f;

    sf::Vector2f pos = getPosition();

    return {
            {pos.x - boxWidth / 2.f, pos.y - boxHeight / 2.f},
            {boxWidth, boxHeight}
    };
}

void ChaserEnemy::draw(sf::RenderWindow& window){
    window.draw(m_sprite);
    window.draw(m_healthBarBackground);
    window.draw(m_healthBarForeground);
}

void ChaserEnemy::takeDamage(float damage) {
    m_health.takeDamage(damage);
}

bool ChaserEnemy::isDead() const {
    return m_health.isDead();
}

void ChaserEnemy::setPosition(sf::Vector2f position) {
    m_sprite.setPosition(position);
}

sf::Vector2f ChaserEnemy::getPosition() const {
    return m_sprite.getPosition();
}

bool ChaserEnemy::isAttacking() const {
    return m_currentState == State::ATTACKING;
}

bool ChaserEnemy::didAttackLand() {
    if (m_didAttackLand) {
        m_didAttackLand = false;
        return true;
    }
    return false;
}

sf::FloatRect ChaserEnemy::getAttackHitbox() const {
    constexpr float hitboxWidth = 100.f;
    constexpr float hitboxHeight = 30.f;
    constexpr float attackDistance = 60.f;

    sf::Vector2f direction(std::cos(m_currentAngleRad), std::sin(m_currentAngleRad));

    sf::Vector2f hitboxCenter = getPosition() + direction * attackDistance;

    return {
        {hitboxCenter.x - hitboxWidth / 2.f, hitboxCenter.y - hitboxHeight / 2.f},
        {hitboxWidth, hitboxHeight}
    };
}

void ChaserEnemy::updateAnimation()
{
    static State previousState = m_currentState;

    if (m_currentState != previousState) {
        m_currentFrame = 0;
        previousState = m_currentState;
        m_animationTimer.restart();
    }

    if (m_animations[m_currentState].empty()) {
        return;
    }

    if (m_animationTimer.getElapsedTime().asSeconds() > m_frameTime)
    {
        m_currentFrame++;

        if (m_currentState == State::ATTACKING && m_currentFrame == m_damageFrame)
        {
            m_didAttackLand = true;
        }

        size_t frameCount = m_animations[m_currentState].size();

        if (static_cast<size_t>(m_currentFrame) >= frameCount)
        {
            if (m_currentState == State::ATTACKING)
            {
                m_currentState = State::IDLE;
                m_currentFrame = 0;
            }
            else
            {
                m_currentFrame = 0;
            }
        }
        m_sprite.setTextureRect(m_animations[m_currentState][m_currentFrame]);
        m_animationTimer.restart();
    }
}
