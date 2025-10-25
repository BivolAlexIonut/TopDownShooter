#include "Health.h"
#include <ostream>

Health::Health() : m_currentHealth(100.f), m_maxHealth(100.f) {}

Health::Health(float maxHealth) : m_currentHealth(maxHealth), m_maxHealth(maxHealth) {}

Health::Health(float currentHealth, float maxHealth)
    : m_currentHealth(currentHealth), m_maxHealth(maxHealth) {
    if (m_currentHealth > m_maxHealth) {
        m_currentHealth = m_maxHealth;
    }
}

Health::~Health() = default;

std::ostream& operator<<(std::ostream& os, const Health& health) {
    os << "Health(" << health.m_currentHealth << "/" << health.m_maxHealth << ")";
    return os;
}
