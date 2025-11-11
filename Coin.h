#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Coin {
public:
    explicit Coin(sf::Vector2f startPos);

    static bool initAssets();

    void update();
    void draw(sf::RenderWindow &window) const;
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);

    [[nodiscard]] bool isDead() const;

private:
    enum class State {
        ALIVE,
        BLINKING,
        DEAD
    };

    sf::Sprite m_sprite;
    int m_currentFrame;
    float m_animSpeed;
    sf::Clock m_animTimer;
    std::vector<sf::IntRect> m_animRects;

    sf::Clock m_lifeTimer;
    sf::Clock m_blinkToggleTimer;

    State m_currentState;
    bool m_isDead;
    bool m_isVisible;

    static const float m_blinkStartTime;
    static const float m_totalLifeTime;
    static const float m_blinkInterval;

    static sf::Texture s_texture;
};