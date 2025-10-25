#pragma once
#include <ostream>
#include <algorithm>

class Health {
public:
    Health();
    explicit Health(float maxHealth);
    Health(float currentHealth, float maxHealth);
    ~Health();

    [[maybe_unused]] void takeDamage(float amount) {
        m_currentHealth -= amount;
        m_currentHealth = std::max(0.f, m_currentHealth);
    }

    [[maybe_unused]] [[nodiscard]] float getPercentage() const {
        if (m_maxHealth <= 0.f) return 0.f;
        return m_currentHealth / m_maxHealth;
    }


    friend std::ostream& operator<<(std::ostream& os, const Health& health);

private:
    float m_currentHealth;
    float m_maxHealth;
};