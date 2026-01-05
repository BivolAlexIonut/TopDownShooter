#pragma once
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Effect.h"

class EffectManager {
public:
    void update();
    void draw(sf::RenderWindow& window);
    void clear();
    void addEffect(std::unique_ptr<Effect> effect);

private:
    std::vector<std::unique_ptr<Effect>> m_effects;
};
