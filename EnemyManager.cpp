#include "EnemyManager.h"

EnemyManager::EnemyManager(std::unique_ptr<EnemyBase> enemy) 
    : m_enemy(std::move(enemy)) 
{
}

EnemyManager::EnemyManager(const EnemyManager& other) {
    if (other.m_enemy) {
        m_enemy = other.m_enemy->clone();
    } else {
        m_enemy = nullptr;
    }
}

EnemyManager& EnemyManager::operator=(const EnemyManager& other) {
    if (this != &other) {
        if (other.m_enemy) {
            m_enemy = other.m_enemy->clone();
        } else {
            m_enemy = nullptr;
        }
    }
    return *this;
}

EnemyBase* EnemyManager::operator->() {
    return m_enemy.get();
}

const EnemyBase* EnemyManager::operator->() const {
    return m_enemy.get();
}

EnemyBase& EnemyManager::operator*() {
    return *m_enemy;
}

const EnemyBase& EnemyManager::operator*() const {
    return *m_enemy;
}

EnemyBase* EnemyManager::get() {
    return m_enemy.get();
}

const EnemyBase* EnemyManager::get() const {
    return m_enemy.get();
}

EnemyManager::operator bool() const {
    return m_enemy != nullptr;
}
