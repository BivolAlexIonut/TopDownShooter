#include "Bullet.h"
#include <cmath>

const float PI = 3.14159265358979323846f;

Bullet::Bullet(sf::Texture &texture, const std::vector<sf::IntRect> &animRects, sf::Vector2f startPos, sf::Vector2f direction,
    float animSpeed)
    : bulletSprite(texture),
      bulletVelocity{},
      bulletRect{},
      bulletTimer{},
      m_animRects(animRects),
      bulletCurrentFrame(0),
      bulletAnimSpeed(animSpeed),
      m_animFrames(static_cast<int>(animRects.size()))
{
    bulletSprite.setTextureRect(m_animRects[0]);

    float originX = static_cast<float>(m_animRects[0].size.x) / 2.f;
    float originY = static_cast<float>(m_animRects[0].size.y) / 2.f;
    bulletSprite.setOrigin({originX, originY});

    float angleInRadians = std::atan2(direction.y, direction.x);
    float angleInDegrees = angleInRadians * (180.f / PI);
    bulletSprite.setRotation(sf::degrees(angleInDegrees));

    bulletSprite.setPosition(startPos);
    float speed = 1500.f;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0)
        direction /= length;
    bulletVelocity = direction * speed;
}

Bullet::~Bullet()= default;

void Bullet::update(float dt) {
    //miscarea glontului
    bulletSprite.move(bulletVelocity*dt);

    if (bulletTimer.getElapsedTime().asSeconds() > bulletAnimSpeed){
        bulletCurrentFrame++;
        bulletTimer.restart();
    }
    if (static_cast<size_t>(bulletCurrentFrame) >= m_animRects.size()) {
        bulletCurrentFrame = 0;
    }

    bulletSprite.setTextureRect(m_animRects[bulletCurrentFrame]);
}

void Bullet::draw(sf::RenderWindow &window) {
    window.draw(bulletSprite);
}