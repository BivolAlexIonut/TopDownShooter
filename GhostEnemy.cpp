#include "GhostEnemy.h"
#include <stdexcept>
#include <cmath>
#include <iostream>

#include "GameExceptions.h"
#include "RandomGenerator.h"

std::map<GhostEnemy::State, sf::Texture> GhostEnemy::s_stateTextures;
std::map<GhostEnemy::AttackDirection, std::vector<sf::IntRect>> GhostEnemy::s_attackAnimations;
sf::Texture GhostEnemy::s_deathTexture;

bool GhostEnemy::initAssets()
{
    if (!s_stateTextures[State::IDLE].loadFromFile("assets/enemies/Spectre_15_Idle.png")) {
         throw AssetLoadException("Spectre_15_Idle.png");
    }
    if (!s_stateTextures[State::MOVING].loadFromFile("assets/enemies/Spectre_15_Move.png")) {
         throw AssetLoadException("Spectre_15_Move.png");
    }
    if (!s_stateTextures[State::ATTACKING].loadFromFile("assets/enemies/Spectre_15_Attack.png")) {
         throw AssetLoadException("Spectre_15_Attack.png!");
    }
    if (!s_stateTextures[State::ABILITY].loadFromFile("assets/enemies/Spectre_15_Ability.png")) {
         throw AssetLoadException("Spectre_15_Ability.png!");
    }

    if (!s_deathTexture.loadFromFile("assets/enemies/death_animation-ghost.png")) {
         throw AssetLoadException("death_animation-ghost.png!");
    }

    const int FRAME_W = 128;
    const int FRAME_H = 128;
    const int ATTACK_COLS = 6;

    s_attackAnimations[AttackDirection::DOWN].clear();
    for (int i = 0; i < ATTACK_COLS; ++i) {
        s_attackAnimations[AttackDirection::DOWN].push_back(sf::IntRect({i * FRAME_W, 0}, {FRAME_W, FRAME_H}));
    }
    s_attackAnimations[AttackDirection::LEFT].clear();
    for (int i = 0; i < ATTACK_COLS; ++i) {
        s_attackAnimations[AttackDirection::LEFT].push_back(sf::IntRect({i * FRAME_W, FRAME_H}, {FRAME_W, FRAME_H}));
    }
    s_attackAnimations[AttackDirection::RIGHT].clear();
    for (int i = 0; i < ATTACK_COLS; ++i) {
        s_attackAnimations[AttackDirection::RIGHT].push_back(sf::IntRect({i * FRAME_W, FRAME_H * 2}, {FRAME_W, FRAME_H}));
    }
    s_attackAnimations[AttackDirection::UP].clear();
    for (int i = 0; i < ATTACK_COLS; ++i) {
        s_attackAnimations[AttackDirection::UP].push_back(sf::IntRect({i * FRAME_W, FRAME_H * 3}, {FRAME_W, FRAME_H}));
    }
    return true;
}

GhostEnemy::GhostEnemy(const std::map<std::string, sf::SoundBuffer> &soundBuffers)
    : m_sprite(GhostEnemy::s_stateTextures[State::IDLE]),
      m_currentState(State::IDLE),
      m_previousState(m_currentState), m_lockedAttackDirection(),
      m_currentFrame(0),
      m_frameTime(0.08f),
      m_speed(80.f),
      m_damageFrame(5),
      m_currentAngleRad(0.f),
      m_didAttackLand(false),
      m_health{75.f, 75.f},
      m_isReadyForRemoval(false),m_justDied(false),m_abilityChargeTime(1.0f),m_moveCooldown(0.45f), m_wasMoving(false) {
    m_sprite.setTexture(s_stateTextures[State::IDLE]);

    if (soundBuffers.count("ghost_move")) {
        m_moveSound.setBuffer(soundBuffers.at("ghost_move"));
        m_moveSound.setVolume(40.f);
    } else {
        std::cerr << "EROARE: Nu am gasit 'ghost_move' in soundBuffers!" << std::endl;
    }

    const int FRAME_W = 128;
    const int FRAME_H = 128;

    const int OTHER_COLS = 6;
    const int OTHER_ROWS = 4;
    const int TOTAL_OTHER_FRAMES = OTHER_COLS * OTHER_ROWS;

    m_animations[State::IDLE].clear();
    for (int i = 0; i < TOTAL_OTHER_FRAMES; ++i) {
        int row = i / OTHER_COLS; int col = i % OTHER_COLS;
        m_animations[State::IDLE].push_back(sf::IntRect({col * FRAME_W, row * FRAME_H}, {FRAME_W, FRAME_H}));
    }

    m_animations[State::MOVING].clear();
    for (int i = 0; i < TOTAL_OTHER_FRAMES; ++i) {
        int row = i / OTHER_COLS; int col = i % OTHER_COLS;
        m_animations[State::MOVING].push_back(sf::IntRect({col * FRAME_W, row * FRAME_H}, {FRAME_W, FRAME_H}));
    }

    m_animations[State::ATTACKING].clear();

    m_animations[State::ABILITY].clear();
    for (int i = 0; i < TOTAL_OTHER_FRAMES; ++i) {
        int row = i / OTHER_COLS; int col = i % OTHER_COLS;
        m_animations[State::ABILITY].push_back(sf::IntRect({col * FRAME_W, row * FRAME_H}, {FRAME_W, FRAME_H}));
    }

    m_animations[State::DYING].clear();
    const int DEATH_FRAME_W = 256; const int DEATH_FRAME_H = 256;
    const int DEATH_COLS = 8; const int DEATH_TOTAL_FRAMES = 48;
    for (int i = 0; i < DEATH_TOTAL_FRAMES; ++i) {
        int row = i / DEATH_COLS; int col = i % DEATH_COLS;
        m_animations[State::DYING].push_back(sf::IntRect({col * DEATH_FRAME_W, row * DEATH_FRAME_H}, {DEATH_FRAME_W, DEATH_FRAME_H}));
    }

    const float HEALTH_BAR_WIDTH = 50.f;
    const float HEALTH_BAR_HEIGHT = 5.f;
    m_healthBarBackground.setSize({HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT});
    m_healthBarBackground.setFillColor(sf::Color(100, 100, 100, 150));
    m_healthBarBackground.setOrigin({HEALTH_BAR_WIDTH / 2.f, HEALTH_BAR_HEIGHT / 2.f});
    m_healthBarForeground.setSize({HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT});
    m_healthBarForeground.setFillColor(sf::Color(255, 0, 0, 200));
    m_healthBarForeground.setOrigin({HEALTH_BAR_WIDTH / 2.f, HEALTH_BAR_HEIGHT / 2.f});

    m_sprite.setTextureRect(m_animations[State::IDLE][0]);
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    m_sprite.setScale({3.f, 3.f});
}


void GhostEnemy::update(sf::Time dt, sf::Vector2f playerPosition, const GameMap& gameMap)
{
    if (m_currentState == State::DYING)
    {
        updateAnimation();
        return;
    }

    sf::Vector2f currentPosition = m_sprite.getPosition();
    sf::Vector2f direction = playerPosition - currentPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    const float attackRange = 200.f;
    const float abilityRange = 1100.f;
    const float moveRange = 500.f;
    const float teleportOffset = 150.f;

    if (m_currentState == State::ATTACKING)
    {
    }
    else if (m_currentState == State::ABILITY)
    {
        if (m_abilityTimer.getElapsedTime().asSeconds() > m_abilityChargeTime)
        {
            m_currentState = State::ATTACKING;
            m_currentFrame = 0;
            m_animationTimer.restart();

            m_animations[State::ATTACKING] = s_attackAnimations[m_lockedAttackDirection];
        }
    }
    else if (m_currentState == State::MOVING)
    {
        if (length < abilityRange && length > attackRange)
        {
            m_currentState = State::ABILITY;
            m_currentFrame = 0;
            m_animationTimer.restart();
            m_abilityTimer.restart();

            float absX = std::abs(direction.x);
            float absY = std::abs(direction.y);
            sf::Vector2f teleportDest;

            if (absX > absY) {
                if (direction.x > 0) {
                    teleportDest = {playerPosition.x + teleportOffset, playerPosition.y};
                    m_lockedAttackDirection = AttackDirection::LEFT;
                } else {
                    teleportDest = {playerPosition.x - teleportOffset, playerPosition.y};
                    m_lockedAttackDirection = AttackDirection::RIGHT;
                }
            } else {
                if (direction.y > 0) {
                    teleportDest = {playerPosition.x, playerPosition.y + teleportOffset};
                    m_lockedAttackDirection = AttackDirection::UP;
                } else {
                    teleportDest = {playerPosition.x, playerPosition.y - teleportOffset};
                    m_lockedAttackDirection = AttackDirection::DOWN;
                }
            }
            const sf::FloatRect mapBounds = gameMap.getPixelBounds();
            teleportDest.x = std::clamp(teleportDest.x, mapBounds.position.x + 1.f, (mapBounds.position.x + mapBounds.size.x) - 1.f);
            teleportDest.y = std::clamp(teleportDest.y, mapBounds.position.y + 1.f, (mapBounds.position.y + mapBounds.size.y) - 1.f);

            setPosition(teleportDest);
        }
        else if (length < attackRange)
        {
            m_currentState = State::ATTACKING;
            m_currentFrame = 0;
            m_animationTimer.restart();

            float absX = std::abs(direction.x);
            float absY = std::abs(direction.y);
            if (absX > absY) {
                m_lockedAttackDirection = (direction.x > 0) ? AttackDirection::RIGHT : AttackDirection::LEFT;
            } else {
                m_lockedAttackDirection = (direction.y > 0) ? AttackDirection::DOWN : AttackDirection::UP;
            }
            m_animations[State::ATTACKING] = s_attackAnimations[m_lockedAttackDirection];
        }
    }
    else if (m_currentState == State::IDLE)
    {
        if (length < moveRange) m_currentState = State::MOVING;
    }


    sf::Vector2f velocity;
    if (m_currentState == State::MOVING)
    {
        sf::Vector2f normalizedDirection(0.f, 0.f);
        if (length != 0) normalizedDirection = direction / length;
        velocity = normalizedDirection * m_speed;

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
    }
    m_currentAngleRad = std::atan2(direction.y, direction.x);

    bool isMoving = (m_currentState == State::MOVING);

    if (isMoving) {
        if (!m_wasMoving) {
            m_moveSound.setPitch(RandomGenerator::getFloat(0.9f, 1.1f));
            m_moveSound.setVolume(10.f);
            m_moveSound.play();
            m_moveTimer.restart();
        } else {
            if (m_moveTimer.getElapsedTime().asSeconds() > m_moveCooldown) {
                m_moveSound.setPitch(RandomGenerator::getFloat(0.9f, 1.1f));
                m_moveSound.setVolume(10.f);
                m_moveSound.play();
                m_moveTimer.restart();
            }
        }
    }
    m_wasMoving = isMoving;

    updateAnimation();
    updateHealthBar();
}

void GhostEnemy::updateAnimation()
{
    if (m_currentState != m_previousState)
    {
        m_currentFrame = 0;
        m_previousState = m_currentState;
        m_animationTimer.restart();

        if (m_currentState == State::ATTACKING)
        {
            m_sprite.setTexture(s_stateTextures[State::ATTACKING]);
            m_animations[State::ATTACKING] = s_attackAnimations[m_lockedAttackDirection];
        }
        else if (m_currentState != State::DYING)
        {
            m_sprite.setTexture(s_stateTextures[m_currentState]);
        }

        if (m_currentState != State::DYING)
        {
            const int FRAME_W = 128;
            const int FRAME_H = 128;
            m_sprite.setOrigin({FRAME_W / 2.f, FRAME_H / 2.f});
            m_sprite.setScale({3.f, 3.f});
        }
        if (!m_animations[m_currentState].empty()) {
            m_sprite.setTextureRect(m_animations[m_currentState][0]);
        }
    }

    float currentFrameTime = m_frameTime;

    if (m_currentState == State::IDLE)
    {
        currentFrameTime = 0.04f;
    }
    else if (m_currentState == State::ABILITY)
    {
        currentFrameTime = 0.15f;
    }
    else if (m_currentState == State::DYING)
    {
        currentFrameTime = 0.025f;
    }

    if (m_animationTimer.getElapsedTime().asSeconds() > currentFrameTime)
    {
        m_currentFrame++;

        if (m_currentState == State::ATTACKING && m_currentFrame == m_damageFrame) {
            m_didAttackLand = true;
        }

        size_t frameCount = m_animations[m_currentState].size();

        if (static_cast<size_t>(m_currentFrame) >= frameCount)
        {
            if (m_currentState == State::ATTACKING)
            {
                m_currentState = State::MOVING;
                m_currentFrame = 0;
            }
            else if (m_currentState == State::ABILITY){}
            else if (m_currentState == State::DYING)
            {
                m_currentFrame = static_cast<int>(frameCount - 1);
                m_isReadyForRemoval = true;
            }
            else
            {
                m_currentFrame = 0;
            }
        }

        if (!m_animations[m_currentState].empty()) {
             m_sprite.setTextureRect(m_animations[m_currentState][m_currentFrame]);
        }
        m_animationTimer.restart();
    }
}

void GhostEnemy::draw(sf::RenderWindow& window) {
    window.draw(m_sprite);
    if (m_currentState != State::DYING) {
        window.draw(m_healthBarBackground);
        window.draw(m_healthBarForeground);
    }
}

void GhostEnemy::setPosition(sf::Vector2f position) {
    m_sprite.setPosition(position);
}

sf::Vector2f GhostEnemy::getPosition() const {
    return m_sprite.getPosition();
}

sf::FloatRect GhostEnemy::getBounds() const {

    if (m_currentState == State::DYING)
        return {};

    const float boxWidth = 30.f;
    const float boxHeight = 50.f;
    sf::Vector2f pos = m_sprite.getPosition();
    return {
        {pos.x - boxWidth / 2.f, pos.y - boxHeight / 2.f},
        {boxWidth, boxHeight}
    };
}

bool GhostEnemy::isAttacking() const {
    return m_currentState == State::ATTACKING;
}

bool GhostEnemy::didAttackLand() {
    if (m_didAttackLand) {
        m_didAttackLand = false;
        return true;
    }
    return false;
}

sf::FloatRect GhostEnemy::getAttackHitbox() const
{
    sf::Vector2f enemyPos = getPosition();
    const float attackWidth = 64.f;
    const float attackLength = 200.f;
    const float attackOffset = 30.f;

    sf::Vector2f hitboxSize;
    sf::Vector2f hitboxTopLeft;

    switch (m_lockedAttackDirection)
    {
        case AttackDirection::UP:
            hitboxSize = {attackWidth, attackLength};
            hitboxTopLeft = {
                enemyPos.x - attackWidth / 2.f,
                enemyPos.y - attackOffset - attackLength
            };
            break;
        case AttackDirection::DOWN:
            hitboxSize = {attackWidth, attackLength};
            hitboxTopLeft = {
                enemyPos.x - attackWidth / 2.f,
                enemyPos.y + attackOffset
            };
            break;
        case AttackDirection::LEFT:
            hitboxSize = {attackLength, attackWidth};
            hitboxTopLeft = {
                enemyPos.x - attackOffset - attackLength,
                enemyPos.y - attackWidth / 2.f
            };
            break;
        case AttackDirection::RIGHT:
            hitboxSize = {attackLength, attackWidth};
            hitboxTopLeft = {
                enemyPos.x + attackOffset,
                enemyPos.y - attackWidth / 2.f
            };
            break;
    }

    return {hitboxTopLeft, hitboxSize};
}

void GhostEnemy::takeDamage(float damage) {
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
        m_sprite.setScale({0.5f, 0.5f});
        m_sprite.setRotation(sf::degrees(0.f));

        if (!m_animations[State::DYING].empty()) {
            m_sprite.setTextureRect(m_animations[State::DYING][0]);
        }
    }
}

bool GhostEnemy::isDead() const {
    return m_isReadyForRemoval;
}

bool GhostEnemy::hasJustDied() const {
    return m_justDied;
}

void GhostEnemy::acknowledgeDeath() {
    m_justDied = false;
}

void GhostEnemy::updateHealthBar() {
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

[[nodiscard]] std::unique_ptr<EnemyBase> GhostEnemy::clone() const {
    return std::make_unique<GhostEnemy>(*this);
}

int GhostEnemy::getCoinValue() const
{
    return 7;
}