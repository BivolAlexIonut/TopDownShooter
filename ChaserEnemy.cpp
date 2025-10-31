#include "ChaserEnemy.h"
#include <cmath>
#include <stdexcept>

ChaserEnemy::ChaserEnemy()
    : m_speed(270.f),
      m_sprite(this->m_texture),
      m_currentState(State::IDLE),
      m_currentFrame(0), m_frameTime(0.05f),m_didAttackLand(false),m_damageFrame(8),m_currentAngleRad(0.f) {

    if (!m_texture.loadFromFile("assets/enemies/chaser_spritesheet_bun_final.png")) {

        throw std::runtime_error("EROARE: Nu am putut incarca chaser_spritesheet.png!");
    }
    m_sprite.setTexture(m_texture);

    constexpr int FRAME_WIDTH = 288;
    constexpr int FRAME_HEIGHT = 311;

    std::vector<sf::IntRect> movingFrames;
    const int TOP_Y_MOVE = 0;
    for (int i = 0; i < 16; ++i) {
        int left_x = i * FRAME_WIDTH;
        movingFrames.push_back(sf::IntRect({left_x, TOP_Y_MOVE}, {FRAME_WIDTH, FRAME_HEIGHT}));
    }

    std::vector<sf::IntRect> idleFrames;
    const int TOP_Y_IDLE = FRAME_HEIGHT;
    for (int i = 0; i < 16; ++i) {
        int left_x = i * FRAME_WIDTH;
        idleFrames.push_back(sf::IntRect({left_x, TOP_Y_IDLE}, {FRAME_WIDTH, FRAME_HEIGHT}));
    }

    std::vector<sf::IntRect> attackingFrames;
    const int TOP_Y_ATTACK = FRAME_HEIGHT * 2;
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


void ChaserEnemy::update(sf::Time dt, sf::Vector2f playerPosition)
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

    if (m_currentState == State::MOVING)
    {
        sf::Vector2f normalizedDirection(0.f, 0.f);
        if (length != 0)
        {
            normalizedDirection = direction / length;
        }
        sf::Vector2f movement = normalizedDirection * m_speed * dt.asSeconds();
        m_sprite.move(movement);
    }
    sf::Vector2f dir = playerPosition - getPosition();
    m_currentAngleRad = std::atan2(dir.y, dir.x);
    float angleInDegrees = m_currentAngleRad * (180.f / 3.14159265f);
    m_sprite.setRotation(sf::degrees(angleInDegrees));

    updateAnimation();
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

bool ChaserEnemy::didAttackLand() {
    if (m_didAttackLand) {
        m_didAttackLand = false;
        return true;
    }
    return false;
}

sf::FloatRect ChaserEnemy::getAttackHitbox() const {
    const float hitboxWidth = 100.f;
    const float hitboxHeight = 30.f;
    const float attackDistance = 60.f;

    sf::Vector2f direction(std::cos(m_currentAngleRad), std::sin(m_currentAngleRad));

    sf::Vector2f hitboxCenter = getPosition() + direction * attackDistance;

    return {
        {hitboxCenter.x - hitboxWidth / 2.f, hitboxCenter.y - hitboxHeight / 2.f},
        {hitboxWidth, hitboxHeight}
    };
}


bool ChaserEnemy::isAttacking() const {
    return m_currentState == State::ATTACKING;
}


void ChaserEnemy::draw(sf::RenderWindow& window){
    window.draw(m_sprite);
}

void ChaserEnemy::setPosition(sf::Vector2f position) {
    m_sprite.setPosition(position);
}

sf::Vector2f ChaserEnemy::getPosition() const {
    return m_sprite.getPosition();
}

sf::FloatRect ChaserEnemy::getBounds() const {
    return m_sprite.getGlobalBounds();
}