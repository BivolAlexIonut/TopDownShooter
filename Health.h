#pragma once
#include <ostream>
#include <algorithm>

class Health {
public:
    Health();

    explicit Health(float maxHealth);

    Health(float currentHealth, float maxHealth);

    ~Health() = default;

    Health(const Health &other); //Constructor de copiere
    Health &operator=(const Health &other); //Operator= de copiere

    [[maybe_unused]] void takeDamage(float amount) {
        m_currentHealth -= amount;
        m_currentHealth = std::max(0.f, m_currentHealth);
    }

    [[maybe_unused]] [[nodiscard]] float getPercentage() const {
        if (m_maxHealth <= 0.f) return 0.f;
        return m_currentHealth / m_maxHealth;
    }

    [[nodiscard]] bool isDead() const {
        return m_currentHealth <= 0.f;
    }

    void addHealth(float amount) {
        m_currentHealth += amount;
        m_currentHealth = std::min(m_maxHealth, m_currentHealth);
    }

    [[nodiscard]] bool isFull() const {
        return m_currentHealth >= m_maxHealth;
    }

    friend std::ostream &operator<<(std::ostream &os, const Health &health);

private:
    float m_currentHealth;
    float m_maxHealth;
};
