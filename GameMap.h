#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class GameMap {
public:
    GameMap();
    ~GameMap();

    bool load(const std::string& jsonPath, const std::string& tilesetPath);
    void draw(sf::RenderWindow& window);

    friend std::ostream& operator<<(std::ostream& os, const GameMap& map);

private:
    sf::Texture m_tilesetTexture;
    std::vector<sf::Sprite> m_tiles;

    sf::Vector2u m_tileSize;
    sf::Vector2u m_mapSize;
};