#include "ResourceManager.h"
#include "GameExceptions.h"
#include <iostream>

ResourceManager::ResourceManager() = default;

void ResourceManager::loadAll() {
    loadTexture("crosshair", "assets/ui/crosshair.png");
    loadTexture("coin", "assets/coin.png");
    loadTexture("ghost_impact", "assets/enemies/death_animation-ghost.png");
    loadTexture("blood_effect", "assets/enemies/death_animation.png");

    if (!m_font.openFromFile("fonts/m6x11.ttf")) {
        throw FontLoadException("fonts/m6x11.ttf");
    }

    loadSound("player_step", "assets/sounds/steps/Steps_floor-001.wav");
    loadSound("pistol_shoot", "assets/sounds/shoot/pistol.wav");
    loadSound("chaser_hurt", "assets/sounds/enemies/chaser/Orc_Damage.wav");
    loadSound("ghost_hurt", "assets/sounds/enemies/ghost/Ghost_Death.wav");
    loadSound("devil_hurt", "assets/sounds/enemies/devil/Behemoth_Death.wav");
    loadSound("chaser_attack", "assets/sounds/enemies/chaser/Attack.wav");
    loadSound("ghost_attack", "assets/sounds/enemies/ghost/Attack.wav");
    loadSound("devil_attack", "assets/sounds/enemies/devil/Attack.wav");
    loadSound("chaser_move", "assets/sounds/enemies/chaser/Move.wav");
    loadSound("ghost_move", "assets/sounds/enemies/chaser/Move.wav");
    loadSound("devil_move", "assets/sounds/enemies/chaser/Move.wav");
    loadSound("pistol_reload", "assets/sounds/reload/pistol_reload.wav");
    loadSound("tommygun_shoot", "assets/sounds/shoot/tommygun.wav");
    loadSound("tommygun_reload", "assets/sounds/reload/tommygun_reload.wav");
    loadSound("rpg_shoot", "assets/sounds/shoot/rpg.wav");
    loadSound("rpg_reload", "assets/sounds/reload/rpg_reload.wav");
    loadSound("smg_shoot", "assets/sounds/shoot/smg.wav");
    loadSound("smg_reload", "assets/sounds/reload/smg_reload.wav");
    loadSound("shotgun_shoot", "assets/sounds/shoot/shotgun.wav");
    loadSound("shotgun_reload", "assets/sounds/reload/shotgun_reload.wav");
    loadSound("sniper_shoot", "assets/sounds/shoot/sniper.wav");
    loadSound("sniper_reload", "assets/sounds/reload/sniper_reload.wav");
    loadSound("coin_drop", "assets/sounds/coin_drop.wav");
    loadSound("coin_pickup", "assets/sounds/coin_pickup.wav");
    loadSound("player_hurt", "assets/sounds/player/damage.wav");

    if (!m_music.openFromFile("assets/sounds/background/retro_metal.ogg")) {
        throw AssetLoadException("retro_metal.ogg");
    }
    m_music.setVolume(60.f);
    m_music.setLooping(true);
}

void ResourceManager::loadSound(const std::string& name, const std::string& path) {
    if (!m_sounds[name].loadFromFile(path)) {
        throw AssetLoadException(path);
    }
}

void ResourceManager::loadTexture(const std::string& name, const std::string& path) {
    if (!m_textures[name].loadFromFile(path)) {
        throw AssetLoadException(path);
    }
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& name) {
    return m_sounds.at(name);
}

std::map<std::string, sf::SoundBuffer>& ResourceManager::getSoundBuffers() {
    return m_sounds;
}

sf::Texture& ResourceManager::getTexture(const std::string& name) {
    return m_textures.at(name);
}

sf::Font& ResourceManager::getFont() {
    return m_font;
}

sf::Music& ResourceManager::getBackgroundMusic() {
    return m_music;
}