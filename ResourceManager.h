#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include "Singleton.h"

class ResourceManager : public Singleton<ResourceManager> {
    friend class Singleton<ResourceManager>;
public:
    void loadAll();

    sf::SoundBuffer& getSoundBuffer(const std::string& name);
    std::map<std::string, sf::SoundBuffer>& getSoundBuffers();
    sf::Texture& getTexture(const std::string& name);
    sf::Font& getFont();
    sf::Music& getBackgroundMusic();

private:
    ResourceManager();
    std::map<std::string, sf::SoundBuffer> m_sounds;
    std::map<std::string, sf::Texture> m_textures;
    sf::Font m_font;
    sf::Music m_music;

    void loadSound(const std::string& name, const std::string& path);
    void loadTexture(const std::string& name, const std::string& path);
};