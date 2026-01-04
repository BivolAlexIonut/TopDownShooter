#pragma once
#include "EnemyBase.h"
#include <memory>

class EnemyManager {
public:
    EnemyManager() = default;
    explicit EnemyManager(std::unique_ptr<EnemyBase> enemy);
    EnemyManager(const EnemyManager& other);
    EnemyManager& operator=(const EnemyManager& other);
    EnemyManager(EnemyManager&& other) noexcept = default;
    EnemyManager& operator=(EnemyManager&& other) noexcept = default;

    ~EnemyManager() = default;

    EnemyBase* operator->();
    const EnemyBase* operator->() const;
    EnemyBase& operator*();
    const EnemyBase& operator*() const;
    
    [[nodiscard]] EnemyBase* get();
    [[nodiscard]] const EnemyBase* get() const;

    explicit operator bool() const;

private:
    std::unique_ptr<EnemyBase> m_enemy;
};
