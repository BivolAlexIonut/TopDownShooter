#pragma once
#ifndef __clangd__
#include <SFML/Graphics.hpp>
#endif
#include <string>
#include <map>

class GameMap : public sf::Drawable, public sf::Transformable {
public:
    GameMap();

    ~GameMap() override = default;

    bool load(const std::string &mapPath, const std::string &tilesetPath, float mapScale);

    sf::FloatRect getPixelBounds() const;

    bool isSolid(sf::Vector2f mousePosition) const;

    sf::Vector2f getTileCenter(sf::Vector2f worldPosition) const;

    [[nodiscard]] int getTileIDAt(sf::Vector2f worldPosition) const;

    sf::Vector2u getTileCoordsAt(sf::Vector2f worldPosition) const;
    bool isPickupOnCooldown(sf::Vector2u tileCoords) const;
    void startPickupCooldown(sf::Vector2u tileCoords);
    void updateAndDrawCooldowns(sf::RenderWindow& window);

    friend std::ostream &operator<<(std::ostream &os, const GameMap &map);

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    sf::Texture m_tilesetTexture;
    sf::VertexArray m_vertices;

    int getTileID(sf::Vector2u tileCoords) const;

    std::vector<int> m_tileIDs;
    sf::Vector2f m_scaledTileSize;

    sf::Vector2u m_tileSize;
    sf::Vector2u m_mapSize;
    sf::Vector2f m_mapPixelSize;

    struct PickupCooldown {
        sf::Clock timer;
    };
    const float m_pickupCooldownDuration = 20.f;

    struct Vector2uComparator {
        bool operator()(const sf::Vector2u &a, const sf::Vector2u &b) const {
            if (a.x != b.x)
                return a.x<b.x;
            return a.y<b.y;
        }
    };
    std::map<sf::Vector2u,PickupCooldown,Vector2uComparator> m_pickupCooldowns;
    sf::RectangleShape m_cooldownBarBg;
    sf::RectangleShape m_cooldownBarFg;
};
