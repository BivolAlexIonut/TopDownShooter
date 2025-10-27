#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameMap : public sf::Drawable ,public sf::Transformable {
public:
    GameMap();
    ~GameMap() override = default;

    bool load(const std::string& mapPath, const std::string& tilesetPath,float mapScale);

    sf::FloatRect getPixelBounds() const;

    friend std::ostream& operator<<(std::ostream& os, const GameMap& map);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Texture m_tilesetTexture; //textura cu toate tileurile
    sf::VertexArray m_vertices; //stocheaza geometria hartii

    sf::Vector2u m_tileSize;
    sf::Vector2u m_mapSize;
    sf::Vector2f m_mapPixelSize;
};
