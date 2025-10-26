#include "GameMap.h"
#include <fstream>
#include <iostream>
#include <ostream>

GameMap::GameMap() : m_tileSize(0, 0), m_mapSize(0, 0) {}

GameMap::~GameMap() = default;

// Am schimbat jsonPath cu mapPath ca sa incarc mapa
bool GameMap::load(const std::string& mapPath, const std::string& tilesetPath) {
    if (!m_tilesetTexture.loadFromFile(tilesetPath)) {
        std::cerr << "EROARE: Nu am putut incarca tileset-ul: " << tilesetPath << std::endl;
        return false;
    }

    std::ifstream f(mapPath);
    if (!f.is_open()) {
        std::cerr << "EROARE: Nu am putut deschide fisierul text al hărții: " << mapPath << std::endl;
        return false;
    }

    const float mapScale = 0.3f; // Ai folosit 0.3f în codul tău

    if (!(f >> m_mapSize.x >> m_mapSize.y)) {
        std::cerr << "EROARE: Nu am putut citi map dimensions din " << mapPath << std::endl;
        f.close();
        return false;
    }

    if (!(f >> m_tileSize.x >> m_tileSize.y)) {
        std::cerr << "EROARE: Nu am putut citi tile dimensions din " << mapPath << std::endl;
        f.close();
        return false;
    }

    std::vector<int> tileIDs;
    int tileID;
    unsigned int totalTiles = m_mapSize.x * m_mapSize.y;
    tileIDs.reserve(totalTiles);

    for (unsigned int i = 0; i < totalTiles; ++i) {
        if (!(f >> tileID)) {
            std::cerr << "EROARE: Fisierul hărții nu conține suficiente ID-uri. Așteptat: " << totalTiles << std::endl;
            f.close();
            return false;
        }
        tileIDs.push_back(tileID);
    }
    f.close();

    unsigned int tilesetColumns = m_tilesetTexture.getSize().x / m_tileSize.x;
    m_tiles.clear();
    m_tiles.reserve(m_mapSize.x * m_mapSize.y);

    for (unsigned int y = 0; y < m_mapSize.y; ++y) {
        for (unsigned int x = 0; x < m_mapSize.x; ++x) {
            int tileIndex = static_cast<int>(y) * static_cast<int>(m_mapSize.x) + static_cast<int>(x);
            int currentTileID = tileIDs[tileIndex];
            if (currentTileID == 0) continue;
            int id = currentTileID - 1;
            int texX = (id % static_cast<int>(tilesetColumns)) * static_cast<int>(m_tileSize.x);
            int texY = (id / static_cast<int>(tilesetColumns)) * static_cast<int>(m_tileSize.y);
            sf::Sprite tileSprite(m_tilesetTexture);
            tileSprite.setTextureRect(sf::IntRect({(int)texX, (int)texY}, {(int)m_tileSize.x, (int)m_tileSize.y}));
            tileSprite.setScale({mapScale, mapScale});
            tileSprite.setPosition({ static_cast<float>(x) * static_cast<float>(m_tileSize.x) * mapScale,
                         static_cast<float>(y) * static_cast<float>(m_tileSize.y) * mapScale });
            m_tiles.push_back(tileSprite);
        }
    }

    std::cout << "Harta incarcata: " << m_mapSize.x << "x" << m_mapSize.y << " piese." << std::endl;
    std::cout << "Am creat " << m_tiles.size() << " sprite-uri pentru harta." << std::endl;
    return true;
}

void GameMap::draw(sf::RenderWindow& window) {
    for (const auto& tile : m_tiles) {
        window.draw(tile);
    }
}

std::ostream& operator<<(std::ostream& os, const GameMap& map) {
    os << "GameMap( Size: " << map.m_mapSize.x << "x" << map.m_mapSize.y
       << " | Tiles: " << map.m_tiles.size() << " )";
    return os;
}
