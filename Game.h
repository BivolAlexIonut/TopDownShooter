#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include "GameMap.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Bullet.h"
#include "Effect.h"
#include "Coin.h"
#include "DevilProjectile.h"
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
    void loadAssets();
    void restartGame();

    sf::RenderWindow m_window;
    sf::View m_camera;
    sf::View m_uiView;
    sf::Clock m_clock;

    GameState m_state;

    // Resurse
    std::map<std::string, sf::SoundBuffer> m_soundBuffers;
    sf::Music m_backgroundMusic;
    sf::Font m_ammoFont;
    
    // Texturi
    sf::Texture m_crosshairTexture;
    sf::Texture m_coinIconTexture;
    sf::Texture m_ghostImpactTexture;
    sf::Texture m_bloodEffectTexture;
    
    // Sprites
    sf::Sprite m_crosshairSprite;
    sf::Sprite m_coinIconSprite;
    sf::Text m_ammoText;
    sf::Text m_coinText;
    
    // Menu Text
    sf::Text m_menuTitle;
    sf::Text m_menuStart;
    sf::Text m_menuExit;
    sf::Text m_gameOverTitle;
    sf::Text m_gameOverRestart;

    // Game object
    GameMap m_gameMap;
    std::unique_ptr<Player> m_player;
    EnemyManager m_enemyManager;
    UpgradeMenu m_upgradeMenu;

    std::vector<Bullet> m_bullets;
    std::vector<std::unique_ptr<Effect>> m_effects;
    std::vector<std::unique_ptr<Coin>> m_coins;
    std::vector<std::unique_ptr<DevilProjectile>> m_enemyProjectiles;
    std::list<sf::Sound> m_activeSounds;

    // Variabile logice
    sf::FloatRect m_mapBounds;
    sf::Clock m_shootTimer;
    sf::Clock m_playerDamageTimer;
    const float m_playerIframeDuration = 0.3f;
    
    // Animation frames
    std::vector<sf::IntRect> m_ghostImpactFrames;
    std::vector<sf::IntRect> m_bloodEffectFrames;
};
