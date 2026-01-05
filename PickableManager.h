#pragma once
#include <vector>
#include <memory>
#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Coin.h"
#include "Player.h"

class PickableManager {
public:
    void update(float dt, Player& player, std::list<sf::Sound>& activeSounds, sf::SoundBuffer& pickupSound);
    void draw(sf::RenderWindow& window) const;
    void clear();
    void addCoin(std::unique_ptr<Coin> coin);

private:
    std::vector<std::unique_ptr<Coin>> m_coins;
    const float m_magnetRadius = 150.f;
    const float m_magnetSpeed = 500.f;
};
