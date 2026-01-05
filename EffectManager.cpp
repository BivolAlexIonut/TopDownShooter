#include "EffectManager.h"

void EffectManager::update() {
    for (auto& effect : m_effects) effect->update();
    std::erase_if(m_effects, [](const auto& e) { return e->isDead(); });
}

void EffectManager::draw(sf::RenderWindow& window) {
    for (const auto& effect : m_effects) effect->draw(window);
}

void EffectManager::clear() {
    m_effects.clear();
}

void EffectManager::addEffect(std::unique_ptr<Effect> effect) {
    m_effects.push_back(std::move(effect));
}
