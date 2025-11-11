#include "Coin.h"
#include "GameExceptions.h"

sf::Texture Coin::s_texture;

const float Coin::m_blinkStartTime = 7.0f;
const float Coin::m_totalLifeTime = 10.0f;
const float Coin::m_blinkInterval = 0.2f;

bool Coin::initAssets() {
    if (!s_texture.loadFromFile("assets/coin.png")) {
        throw AssetLoadException("assets/coin.png");
    }
    return true;
}

Coin::Coin(sf::Vector2f startPos) : m_sprite(Coin::s_texture),m_currentFrame(0),m_animSpeed(0.1f),m_currentState(),
            m_isDead(false),m_isVisible(true) {

    const int FRAME_WIDTH = 20;
    const int FRAME_HEIGHT = 20;
    const int FRAME_COUNT = 9;

    for (int i=0;i<FRAME_COUNT;++i) {
        m_animRects.push_back(sf::IntRect({i*FRAME_WIDTH,0},{FRAME_WIDTH,FRAME_HEIGHT}));
    }

    m_sprite.setTextureRect(m_animRects[0]);
    sf::FloatRect bounds = m_sprite.getGlobalBounds();
    m_sprite.setOrigin({bounds.size.x/2,bounds.size.y/2});
    m_sprite.setPosition(startPos);
    m_sprite.setScale({1.5f,1.5f});

    m_animTimer.restart();
    m_lifeTimer.restart();
}

void Coin::update() {
    if (m_isDead)
        return;

    float elapsedLife = m_lifeTimer.getElapsedTime().asSeconds();

    if (elapsedLife > m_totalLifeTime) {
        m_isDead = true;
        m_isVisible = false;
        return;
    }

    if (elapsedLife > m_blinkStartTime && m_currentState == State::ALIVE) {
        m_currentState = State::BLINKING;
        m_blinkToggleTimer.restart();
    }

    if (m_currentState == State::BLINKING) {
        if (m_blinkToggleTimer.getElapsedTime().asSeconds() > m_blinkInterval) {
            m_isVisible = !m_isVisible;
            m_blinkToggleTimer.restart();
        }
    }

    if (m_animTimer.getElapsedTime().asSeconds() > m_animSpeed) {
        m_currentFrame++;
        m_animTimer.restart();
    }

    if (static_cast<size_t>(m_currentFrame) >= m_animRects.size()) {
        m_currentFrame = 0;
    }

    m_sprite.setTextureRect(m_animRects[m_currentFrame]);
}

void Coin::draw(sf::RenderWindow &window) const {
    if (m_isVisible && !m_isDead) {
        window.draw(m_sprite);
    }
}

sf::Vector2f Coin::getPosition() const
{
    return m_sprite.getPosition();
}

sf::FloatRect Coin::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

void Coin::setPosition(sf::Vector2f pos) {
    m_sprite.setPosition(pos);
}

bool Coin::isDead() const {
    return m_isDead;
}