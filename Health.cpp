#include "Health.h"

#include <iostream>
#include <ostream>

Health::Health() : m_currentHealth(100.f), m_maxHealth(100.f) {
}

Health::Health(float maxHealth) : m_currentHealth(maxHealth), m_maxHealth(maxHealth) {
}

Health::Health(float currentHealth, float maxHealth)
    : m_currentHealth(currentHealth), m_maxHealth(maxHealth) {
    if (m_currentHealth > m_maxHealth) {
        m_currentHealth = m_maxHealth;
    }
}

Health::Health(const Health &other) : m_currentHealth(other.m_currentHealth), m_maxHealth(other.m_maxHealth) {
    std::cout << "Health construcoitr de copiere apelat" << std::endl;
}

Health &Health::operator=(const Health &other) {
    if (this == &other) {
        return *this;
    }

    m_currentHealth = other.m_currentHealth;
    m_maxHealth = other.m_maxHealth;

    std::cout << "Health operator = apelat" << std::endl;

    return *this;
}

std::ostream &operator<<(std::ostream &os, const Health &health) {
    os << "Health(" << health.m_currentHealth << "/" << health.m_maxHealth << ")";
    return os;
}
