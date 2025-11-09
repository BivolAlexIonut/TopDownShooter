#include "DevilEnemy.h"
#include "GameExceptions.h"
#include <cmath>

std::map<DevilEnemy::State, sf::Texture> DevilEnemy::s_textures;

void DevilEnemy::setupAnimation(std::vector<sf::IntRect>& frames, int frameCount)
{
    const int yPos = 0;
    const int frameW = 81;
    const int frameH = 71;

    frames.clear();
    for (int i = 0; i < frameCount; ++i) {
        frames.push_back(sf::IntRect({i * frameW, yPos}, {frameW, frameH}));
    }
}

bool DevilEnemy::initAssets(const std::string& basePath)
{
    if (!s_textures[State::IDLE].loadFromFile(basePath + "IDLE.png")) {
        throw AssetLoadException(basePath + "IDLE.png");
    }
    if (!s_textures[State::FLYING].loadFromFile(basePath + "FLYING.png")) {
        throw AssetLoadException(basePath + "FLYING.png");
    }
    if (!s_textures[State::ATTACKING].loadFromFile(basePath + "ATTACK.png")) {
        throw AssetLoadException(basePath + "ATTACK.png");
    }
    if (!s_textures[State::HURT].loadFromFile(basePath + "HURT.png")) {
        throw AssetLoadException(basePath + "HURT.png");
    }
    if (!s_textures[State::DYING].loadFromFile(basePath + "DEATH.png")) {
        throw AssetLoadException(basePath + "DEATH.png");
    }
    return true;
}

DevilEnemy::DevilEnemy()
    : m_sprite(s_textures[State::IDLE]),
      m_currentState(State::IDLE),
      m_previousState(m_currentState),
      m_currentFrame(0), m_frameTime(0.1f), m_speed(150.f), m_damageFrame(6),
      m_didAttackLand(false),
      m_health(120.f, 120.f),
      m_isReadyForRemoval(false),m_justDied(false)
{
    const int FRAME_W = 81;
    const int FRAME_H = 71;

    setupAnimation(m_animations[State::IDLE], 4);
    setupAnimation(m_animations[State::FLYING], 4);
    setupAnimation(m_animations[State::ATTACKING], 8);
    setupAnimation(m_animations[State::HURT], 4);
    setupAnimation(m_animations[State::DYING], 5);

    const float HEALTH_BAR_WIDTH = 60.f;
    const float HEALTH_BAR_HEIGHT = 5.f;
    m_healthBarBackground.setSize({HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT});
    m_healthBarBackground.setFillColor(sf::Color(100, 100, 100, 150));
    m_healthBarBackground.setOrigin({HEALTH_BAR_WIDTH / 2.f, HEALTH_BAR_HEIGHT / 2.f});
    m_healthBarForeground.setSize({HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT});
    m_healthBarForeground.setFillColor(sf::Color(255, 0, 0, 200));
    m_healthBarForeground.setOrigin({HEALTH_BAR_WIDTH / 2.f, HEALTH_BAR_HEIGHT / 2.f});

    m_sprite.setTextureRect(m_animations[State::IDLE][0]);

    m_sprite.setOrigin({FRAME_W / 2.f, FRAME_H / 2.f});

    m_sprite.setScale({1.2f, 1.2f});
    m_shootTimer.restart();
}


void DevilEnemy::update(sf::Time dt, sf::Vector2f playerPosition, const GameMap& gameMap)
{
    (void)gameMap;

    if (m_currentState == State::DYING) {
        updateAnimation();
        return;
    }

    if (m_currentState == State::HURT) {
        updateAnimation();
        updateHealthBar();
        return;
    }

    sf::Vector2f currentPosition = m_sprite.getPosition();
    sf::Vector2f directionToPlayer = playerPosition - currentPosition;
    float length = std::sqrt(directionToPlayer.x * directionToPlayer.x + directionToPlayer.y * directionToPlayer.y);

    sf::Vector2f normalizedDir(0.f, 0.f);
    if (length != 0.f) {
        normalizedDir = directionToPlayer / length;
    }

    const float attackRange = 450.f;
    const float idealRangeMin = 400.f;
    const float moveRange = 700.f;
    const float attackCooldown = 2.5f;

    if (m_currentState != State::ATTACKING) {
        if (length <= attackRange && length >= idealRangeMin && m_shootTimer.getElapsedTime().asSeconds() > attackCooldown) {
            m_currentState = State::ATTACKING;
            m_shootTimer.restart();
        }
        else if (length < moveRange) {
            m_currentState = State::FLYING;
        }
        else {
            m_currentState = State::IDLE;
        }
    }

    sf::Vector2f velocity(0.f, 0.f);
    if (m_currentState == State::FLYING) {
        if (length < idealRangeMin) {
            velocity = -normalizedDir * m_speed;
        } else if (length > attackRange) {
            velocity = normalizedDir * m_speed;
        }
    }

    m_sprite.move(velocity * dt.asSeconds());

    if (directionToPlayer.x > 1.0f) {
        m_sprite.setScale(sf::Vector2f(-1.2f, 1.2f));
    } else if (directionToPlayer.x < -1.0f) {
        m_sprite.setScale(sf::Vector2f(1.2f, 1.2f));
    }

    updateAnimation();
    updateHealthBar();
}

void DevilEnemy::updateAnimation()
{
    if (m_currentState != m_previousState) {
        m_currentFrame = 0;
        m_previousState = m_currentState;
        m_animationTimer.restart();
        m_sprite.setTexture(s_textures[m_currentState]);
    }

    m_frameTime = (m_currentState == State::ATTACKING) ? 0.1f : 0.15f;
    if (m_currentState == State::DYING) m_frameTime = 0.12f;
    if (m_currentState == State::HURT) m_frameTime = 0.05f;

    if (m_animationTimer.getElapsedTime().asSeconds() > m_frameTime) {
        m_currentFrame++;
        m_animationTimer.restart();

        if (m_currentState == State::ATTACKING && m_currentFrame == m_damageFrame) {
            m_didAttackLand = true;
        }

        size_t frameCount = m_animations[m_currentState].size();
        if (static_cast<size_t>(m_currentFrame) >= frameCount) {
            if (m_currentState == State::ATTACKING || m_currentState == State::HURT) {
                m_currentState = State::FLYING;
                m_currentFrame = 0;
            }
            else if (m_currentState == State::DYING) {
                m_currentFrame = static_cast<int>(frameCount - 1);
                m_isReadyForRemoval = true;
            } else {
                m_currentFrame = 0;
            }
        }
    }

    if (!m_animations[m_currentState].empty()) {
        if (static_cast<size_t>(m_currentFrame) < m_animations[m_currentState].size()) {
            m_sprite.setTextureRect(m_animations[m_currentState][m_currentFrame]);
        }
    }
}

void DevilEnemy::updateHealthBar()
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

void DevilEnemy::draw(sf::RenderWindow& window) {
    window.draw(m_sprite);
    if (m_currentState != State::DYING && m_currentState != State::HURT) {
        window.draw(m_healthBarBackground);
        window.draw(m_healthBarForeground);
    }
}

void DevilEnemy::takeDamage(float damage) {
    if (m_currentState == State::DYING) return;
    m_health.takeDamage(damage);

    if (m_health.isDead()) {
        m_currentState = State::DYING;
        m_justDied = true;
        m_currentFrame = 0;
        m_animationTimer.restart();
        m_sprite.setTexture(s_textures[State::DYING]);
    } else {
        if (m_currentState != State::HURT)
        {
            m_currentState = State::HURT;
            m_currentFrame = 0;
            m_animationTimer.restart();
            m_sprite.setTexture(s_textures[State::HURT]);
        }
    }
}

void DevilEnemy::setPosition(sf::Vector2f position) { m_sprite.setPosition(position); }
sf::Vector2f DevilEnemy::getPosition() const { return m_sprite.getPosition(); }
bool DevilEnemy::isDead() const { return m_isReadyForRemoval; }
bool DevilEnemy::hasJustDied() const { return m_justDied; }
void DevilEnemy::acknowledgeDeath() { m_justDied = false; }
bool DevilEnemy::isAttacking() const { return m_currentState == State::ATTACKING; }

sf::FloatRect DevilEnemy::getBounds() const {

    if (m_currentState == State::DYING)
        return {};

    const float boxWidth = 50.f;
    const float boxHeight = 60.f;
    sf::Vector2f pos = m_sprite.getPosition();
    return {{pos.x - boxWidth / 2.f, pos.y - boxHeight / 2.f}, {boxWidth, boxHeight}};
}

bool DevilEnemy::didAttackLand() {
    if (m_didAttackLand) {
        m_didAttackLand = false;
        return true;
    }
    return false;
}

sf::FloatRect DevilEnemy::getAttackHitbox() const {
    return {{-1.f, -1.f}, {-1.f, -1.f}};
}

/*
std::unique_ptr<EnemyBase> DevilEnemy::clone() const {
    return std::make_unique<DevilEnemy>(*this);
}*/

int DevilEnemy::getCoinValue() const {
    return 10;
}