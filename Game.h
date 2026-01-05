#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <list>
#include <optional>
#include "GameMap.h"
#include "Player.h"
#include "EnemyManager.h"
#include "ResourceManager.h"
#include "ProjectileManager.h"
#include "PickableManager.h"
#include "EffectManager.h"
#include "UpgradeMenu.h"

enum class GameState { MainMenu, Playing, Paused, GameOver };

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time dt);
    void render();
    void restartGame();

    sf::RenderWindow m_window;
    sf::View m_camera;
    sf::View m_uiView;
    sf::Clock m_clock;

    GameState m_state;

    ResourceManager m_resources;
    ProjectileManager m_projectileManager;
    PickableManager m_pickableManager;
    EffectManager m_effectManager;
    EnemyManager m_enemyManager;
    
    std::unique_ptr<Player> m_player;
    GameMap m_gameMap;
    UpgradeMenu m_upgradeMenu;

    std::optional<sf::Sprite> m_crosshairSprite;
    std::optional<sf::Sprite> m_coinIconSprite;
    std::optional<sf::Text> m_ammoText;
    std::optional<sf::Text> m_coinText;
    
    std::optional<sf::Text> m_menuTitle;
    std::optional<sf::Text> m_menuStart;
    std::optional<sf::Text> m_menuExit;
    std::optional<sf::Text> m_gameOverTitle;
    std::optional<sf::Text> m_gameOverRestart;

    std::list<sf::Sound> m_activeSounds;

    sf::FloatRect m_mapBounds;
    sf::Clock m_shootTimer;
    sf::Clock m_playerDamageTimer;
    const float m_playerIframeDuration = 0.3f;
    
    std::vector<sf::IntRect> m_ghostImpactFrames;
    std::vector<sf::IntRect> m_bloodEffectFrames;
};
