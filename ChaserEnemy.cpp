#include "ChaserEnemy.h"
#include "GameExceptions.h"
#include <cmath>
#include <stdexcept>

sf::Texture ChaserEnemy::s_texture;
sf::Texture ChaserEnemy::s_alertTexture;
sf::Texture ChaserEnemy::s_deathTexture;

bool ChaserEnemy::initAssets()
{
    if (!s_texture.loadFromFile("assets/enemies/chaser_spritesheet_bun_final.png"))
    {
        throw AssetLoadException("chaser_spritesheet_bun_final.png");
    }
    if (!s_alertTexture.loadFromFile("assets/ui/alert_icon.png"))
    {
        throw AssetLoadException("alert_icon.png");
    }
    if (!s_deathTexture.loadFromFile("assets/enemies/death_animation.png"))
    {
        throw AssetLoadException("death_animation.png");
    }
    return true;
}

ChaserEnemy::ChaserEnemy()
    : m_speed(270.f),
      m_sprite(ChaserEnemy::s_texture),
      m_currentState(State::IDLE),
      m_currentFrame(0), m_frameTime(0.05f), m_didAttackLand(false), m_damageFrame(8), m_currentAngleRad(0.f),
      m_health(100.f, 100.f), m_previousState(m_currentState), m_alertSprite(ChaserEnemy::s_alertTexture),
      m_alertDuration(0.8f),m_isReadyForRemoval(false),m_justDied(false)
    {

    m_sprite.setTexture(s_texture);
    m_alertSprite.setTexture(s_alertTexture);
    sf::FloatRect iconBounds = m_alertSprite.getLocalBounds();
    m_alertSprite.setOrigin({iconBounds.size.x / 2.f, iconBounds.size.y});
    m_alertSprite.setScale({0.15f, 0.15f});
    m_alertSprite.setColor(sf::Color::Yellow);

    constexpr int FRAME_WIDTH = 288;
    constexpr int FRAME_HEIGHT = 311;
    const float HEALTH_BAR_WIDTH = 50.f;
    const float HEALTH_BAR_HEIGHT = 5.f;
    const int DEATH_FRAME_WIDTH = 256;
    const int DEATH_FRAME_HEIGHT = 256;
    const int DEATH_FRAMES_PER_ROW = 5;
    const int TOTAL_DEATH_FRAMES = 20;
    m_animations[State::DYING].clear();
    for (int i = 0; i < TOTAL_DEATH_FRAMES; ++i) {
        int row = i / DEATH_FRAMES_PER_ROW;
        int col = i % DEATH_FRAMES_PER_ROW;

        m_animations[State::DYING].push_back(
            sf::IntRect({col * DEATH_FRAME_WIDTH, row * DEATH_FRAME_HEIGHT},
                        {DEATH_FRAME_WIDTH, DEATH_FRAME_HEIGHT})
        );
    }

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


void ChaserEnemy::update(sf::Time dt, sf::Vector2f playerPosition, const GameMap& gameMap)
{
    if (m_currentState == State::DYING)
    {
        updateAnimation();
        return;
    }

    sf::Vector2f currentPosition = m_sprite.getPosition();
    sf::Vector2f direction = playerPosition - currentPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    const float attackRange = 60.f;
    const float moveRange = 500.f;

    if (m_currentState == State::IDLE)
    {
        if (length < moveRange)
        {
            m_currentState = State::ALERTED;
            m_alertTimer.restart();
        }
    }
    else if (m_currentState == State::ALERTED)
    {
        if (m_alertTimer.getElapsedTime().asSeconds() > m_alertDuration)
        {
            m_currentState = State::MOVING;
        }
    }
    else if (m_currentState == State::MOVING)
    {
        if (length < attackRange)
        {
            m_currentState = State::ATTACKING;
        }
    }
    else if (m_currentState == State::ATTACKING)
    {

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
    if (m_currentState != State::DYING)
    {
        window.draw(m_healthBarBackground);
        window.draw(m_healthBarForeground);

        if (m_currentState == State::ALERTED)
        {
            sf::FloatRect spriteBounds = m_sprite.getGlobalBounds();
            float alertOffsetY = (spriteBounds.size.y / 2.f) + 20.f;

            m_alertSprite.setPosition({
                m_sprite.getPosition().x,
                m_sprite.getPosition().y - alertOffsetY
            });
            window.draw(m_alertSprite);
        }
    }
}

void ChaserEnemy::takeDamage(float damage) {
    if (m_currentState == State::DYING) return;

    m_health.takeDamage(damage);
    if (m_health.isDead())
    {
        m_currentState = State::DYING;
        m_justDied = true;
        m_currentFrame = 0;
        m_animationTimer.restart();

        m_sprite.setTexture(s_deathTexture);
        m_sprite.setOrigin({128.f, 128.f});

        float scaleValue = 1.f;
        m_sprite.setScale({scaleValue, scaleValue});
        m_sprite.setRotation(sf::degrees(0.f));
    }
}

bool ChaserEnemy::isDead() const {
    return m_isReadyForRemoval;
}

void ChaserEnemy::setPosition(sf::Vector2f position) {
    m_sprite.setPosition(position);
}

sf::Vector2f ChaserEnemy::getPosition() const {
    return m_sprite.getPosition();
}

bool ChaserEnemy::hasJustDied() const {
    return m_justDied;
}

void ChaserEnemy::acknowledgeDeath() {
    m_justDied = false;
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
    if (m_currentState != m_previousState) {
        m_currentFrame = 0;
        m_previousState = m_currentState;
        m_animationTimer.restart();
    }

    float currentFrameTime = m_frameTime;
    if (m_currentState == State::DYING) {
        currentFrameTime = 0.03f;
    }

    if (m_animationTimer.getElapsedTime().asSeconds() > currentFrameTime)
    {
        m_currentFrame++;
        m_animationTimer.restart();
        if (m_currentState == State::ATTACKING && m_currentFrame == m_damageFrame) {
            m_didAttackLand = true;
        }

        size_t frameCount = m_animations[m_currentState].size();
        if (static_cast<size_t>(m_currentFrame) >= frameCount)
        {
            if (m_currentState == State::ATTACKING) {
                m_currentState = State::MOVING;
                m_currentFrame = 0;
            } else if (m_currentState == State::DYING) {
                m_currentFrame = static_cast<int>(frameCount - 1);
                m_isReadyForRemoval = true;
            } else {
                m_currentFrame = 0; // Loop
            }
        }
    }
    if (!m_animations[m_currentState].empty())
    {
        if (static_cast<size_t>(m_currentFrame) < m_animations[m_currentState].size())
        {
            m_sprite.setTextureRect(m_animations[m_currentState][m_currentFrame]);
        }
    }
}

int ChaserEnemy::getCoinValue() const
{
    return 5;
}