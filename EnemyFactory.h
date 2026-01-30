#pragma once
#include <memory>
#include "EnemyBase.h"
#include "ChaserEnemy.h"
#include "GhostEnemy.h"
#include "DevilEnemy.h"
#include "ResourceManager.h"

enum class EnemyType {
    Chaser,
    Ghost,
    Devil
};

class EnemyFactory {
public:
    static std::unique_ptr<EnemyBase> createEnemy(EnemyType type) {
        auto& buffers = ResourceManager::getInstance().getSoundBuffers();
        switch (type) {
            case EnemyType::Chaser: return std::make_unique<ChaserEnemy>(buffers);
            case EnemyType::Ghost: return std::make_unique<GhostEnemy>(buffers);
            case EnemyType::Devil: return std::make_unique<DevilEnemy>(buffers);
            default: return nullptr;
        }
    }
};
