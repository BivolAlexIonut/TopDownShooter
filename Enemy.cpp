#include "Enemy.h"

Enemy::Enemy(float startX, float startY)
    : health(50.f), m_position(startX, startY),
      enemyTexture(),
      enemySprite(this->enemyTexture)
{
}

Enemy::~Enemy() = default;

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(this->enemySprite);
}

void Enemy::update(float dt, sf::Vector2f playerPosition) {
    (void)dt;
    (void)playerPosition;
}

void Enemy::takeDamage(float damage) {
    this->health -= damage;
    if (this->health < 0) this->health = 0;
}

sf::Vector2f Enemy::getPosition() const {
    return this->m_position;
}

std::ostream& operator<<(std::ostream& os, const Enemy& enemy) {
    os << "Enemy( Poz: " << enemy.m_position.x << ", " << enemy.m_position.y
       << " | Health: " << enemy.health << ")";
    return os;
}