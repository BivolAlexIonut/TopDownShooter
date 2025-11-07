#include "GameMap.h"
#include "GameExceptions.h"
#include <fstream>
#include <iostream>
#include <ostream>

GameMap::GameMap() : m_tileSize(0, 0), m_mapSize(0, 0) {
}

bool GameMap::load(const std::string &mapPath, const std::string &tilesetPath, float mapScale) {
    if (!m_tilesetTexture.loadFromFile(tilesetPath)) {
        throw AssetLoadException(tilesetPath);
    }

    std::ifstream f(mapPath);
    if (!f.is_open()) {
        throw MapLoadException(mapPath, "Fisierul text nu a putut fi deschis.");
    }

    if (!(f >> m_mapSize.x >> m_mapSize.y)) {
        throw MapLoadException(mapPath, "Nu am putut citi dimensiunile harti.");
    }

    if (!(f >> m_tileSize.x >> m_tileSize.y)) {
        throw MapLoadException(mapPath, "Nu am putut citi tile dimensions.");
    }

    //citesc id-s dintr un vector temporar
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

    // Numar cate tiles sunt peste podea pentru a putea incarca si alte texturi in afara de
    //cea a podeleip
    size_t objectTileCount = 0;
    for (const int id: tileIDs) {
        if (id > 1) //Aici verific daca textura este sau nu podea si numar
            objectTileCount++;
    }

    // Dimensiunea arrayului ca sa includa podeaua
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize((totalTiles + objectTileCount) * 6); //am doua triunghiuri deci 6 varfuri

    const size_t tilesetColumns = m_tilesetTexture.getSize().x / m_tileSize.x;
    size_t vertexIndex = 0; //index pentryu varf

    const auto fTileSizeX = static_cast<float>(m_tileSize.x);
    const auto fTileSizeY = static_cast<float>(m_tileSize.y);
    const float fScaledTileSizeX = fTileSizeX * mapScale;
    const float fScaledTileSizeY = fTileSizeY * mapScale;

    m_scaledTileSize = {fScaledTileSizeX, fScaledTileSizeY};

    //pentru calculul si memorarea dimensiunilor ca ,camera sa nu iasa din harta
    m_mapPixelSize.x = static_cast<float>(m_mapSize.x) * fScaledTileSizeX;
    m_mapPixelSize.y = static_cast<float>(m_mapSize.y) * fScaledTileSizeY;

    constexpr float floorTexX = 0.f;
    constexpr float floorTexY = 0.f;

    const sf::Vector2f floorTopLeftTex(floorTexX, floorTexY);
    const sf::Vector2f floorTopRightTex(floorTexX + fTileSizeX, floorTexY);
    const sf::Vector2f floorBotRightTex(floorTexX + fTileSizeX, floorTexY + fTileSizeY);
    const sf::Vector2f floorBotLeftTex(floorTexX, floorTexY + fTileSizeY);

    for (unsigned int y = 0; y < m_mapSize.y; ++y) {
        for (unsigned int x = 0; x < m_mapSize.x; ++x) {
            //calculez pozitia pe ecran
            const sf::Vector2f topLeftPos(static_cast<float>(x) * fScaledTileSizeX,
                                          static_cast<float>(y) * fScaledTileSizeY);
            const sf::Vector2f topRightPos(topLeftPos.x + fScaledTileSizeX, topLeftPos.y);
            const sf::Vector2f botRightPos(topLeftPos.x + fScaledTileSizeX, topLeftPos.y + fScaledTileSizeY);
            const sf::Vector2f botLeftPos(topLeftPos.x, topLeftPos.y + fScaledTileSizeY);

            sf::Vertex *tileVertices = &m_vertices[vertexIndex * 6];

            // Primul triunghi
            tileVertices[0].position = topLeftPos;
            tileVertices[1].position = topRightPos;
            tileVertices[2].position = botRightPos;
            tileVertices[0].texCoords = floorTopLeftTex;
            tileVertices[1].texCoords = floorTopRightTex;
            tileVertices[2].texCoords = floorBotRightTex;

            // Al doilea triunghi
            tileVertices[3].position = topLeftPos;
            tileVertices[4].position = botRightPos;
            tileVertices[5].position = botLeftPos;
            tileVertices[3].texCoords = floorTopLeftTex;
            tileVertices[4].texCoords = floorBotRightTex;
            tileVertices[5].texCoords = floorBotLeftTex;

            vertexIndex++; // tec la urmatorul set de 6 varfuri
        }
    }

    //Acum desenex obiectele care sunt deasupra podelei
    for (unsigned int y = 0; y < m_mapSize.y; ++y) {
        for (unsigned int x = 0; x < m_mapSize.x; ++x) {
            int currentTileID = tileIDs[y * m_mapSize.x + x];

            if (currentTileID <= 1) continue;

            int id = currentTileID - 1;

            // calculez iar pozitita pe ecran
            const sf::Vector2f topLeftPos(static_cast<float>(x) * fScaledTileSizeX,
                                          static_cast<float>(y) * fScaledTileSizeY);
            const sf::Vector2f topRightPos(topLeftPos.x + fScaledTileSizeX, topLeftPos.y);
            const sf::Vector2f botRightPos(topLeftPos.x + fScaledTileSizeX, topLeftPos.y + fScaledTileSizeY);
            const sf::Vector2f botLeftPos(topLeftPos.x, topLeftPos.y + fScaledTileSizeY);

            // coordonatele texturii obiectului
            const size_t u = static_cast<size_t>(id) % tilesetColumns;
            const size_t v = static_cast<size_t>(id) / tilesetColumns;
            const auto texX = static_cast<float>(u * m_tileSize.x);
            const auto texY = static_cast<float>(v * m_tileSize.y);

            const sf::Vector2f topLeftTex(texX, texY);
            const sf::Vector2f topRightTex(texX + fTileSizeX, texY);
            const sf::Vector2f botRightTex(texX + fTileSizeX, texY + fTileSizeY);
            const sf::Vector2f botLeftTex(texX, texY + fTileSizeY);

            sf::Vertex *tileVertices = &m_vertices[vertexIndex * 6];

            // Primul triunghi
            tileVertices[0].position = topLeftPos;
            tileVertices[1].position = topRightPos;
            tileVertices[2].position = botRightPos;
            tileVertices[0].texCoords = topLeftTex;
            tileVertices[1].texCoords = topRightTex;
            tileVertices[2].texCoords = botRightTex;

            // Al doilea triunghi
            tileVertices[3].position = topLeftPos;
            tileVertices[4].position = botRightPos;
            tileVertices[5].position = botLeftPos;
            tileVertices[3].texCoords = topLeftTex;
            tileVertices[4].texCoords = botRightTex;
            tileVertices[5].texCoords = botLeftTex;

            vertexIndex++;
        }
    }

    //bara de progres la cooldown
    const float BAR_WIDTH = m_scaledTileSize.x * 0.8f;
    const float BAR_HEIGHT =  8.f;

    m_cooldownBarBg.setSize({BAR_WIDTH, BAR_HEIGHT});
    m_cooldownBarBg.setFillColor(sf::Color(50, 50, 50, 150));
    m_cooldownBarBg.setOrigin({BAR_WIDTH / 2.f, BAR_HEIGHT / 2.f});

    m_cooldownBarFg.setSize({BAR_WIDTH, BAR_HEIGHT});
    m_cooldownBarFg.setFillColor(sf::Color(255, 255, 0, 200));
    m_cooldownBarFg.setOrigin({0, BAR_HEIGHT / 2.f});

    m_tileIDs = std::move(tileIDs);

    std::cout << "Harta incarcata: " << m_mapSize.x << "x" << m_mapSize.y << " piese." << std::endl;
    std::cout << "Am creat " << m_vertices.getVertexCount() << " sprite-uri pentru harta." << std::endl;
    return true;
}

void GameMap::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &m_tilesetTexture;
    target.draw(m_vertices, states);
}

std::ostream &operator<<(std::ostream &os, const GameMap &map) {
    os << "GameMap( Size: " << map.m_mapSize.x << "x" << map.m_mapSize.y
            << " | Tiles: " << map.m_vertices.getVertexCount() / 6 << " )";
    return os;
}

sf::Vector2f GameMap::getTileCenter(sf::Vector2f worldPosition) const
{
    auto tileX = static_cast<unsigned int>(worldPosition.x / m_scaledTileSize.x);
    auto tileY = static_cast<unsigned int>(worldPosition.y / m_scaledTileSize.y);

    float centerX = (static_cast<float>(tileX) * m_scaledTileSize.x) + (m_scaledTileSize.x / 2.f);
    float centerY = (static_cast<float>(tileY) * m_scaledTileSize.y) + (m_scaledTileSize.y / 2.f);
    return {centerX, centerY};
}

sf::FloatRect GameMap::getPixelBounds() const {
    return {{0.f, 0.f}, m_mapPixelSize};
}

int GameMap::getTileIDAt(sf::Vector2f worldPosition) const
{
    if (m_scaledTileSize.x == 0.0f || m_scaledTileSize.y == 0.0f) {
        return 0;
    }
    if (!getPixelBounds().contains(worldPosition)) {
        return 0;
    }

    sf::Vector2u tileCoords;
    tileCoords.x = static_cast<unsigned int>(worldPosition.x / m_scaledTileSize.x);
    tileCoords.y = static_cast<unsigned int>(worldPosition.y / m_scaledTileSize.y);

    return getTileID(tileCoords);
}

int GameMap::getTileID(sf::Vector2u tileCoords) const {
    if (tileCoords.x > m_mapSize.x || tileCoords.y > m_mapSize.y) {
        return 0;
    }
    return m_tileIDs[tileCoords.y * m_mapSize.x + tileCoords.x];
}

bool GameMap::isSolid(sf::Vector2f mousePosition) const {
    if (m_scaledTileSize.x == 0.0f || m_scaledTileSize.y == 0.0f) {
        return false;
    }

    if (!getPixelBounds().contains(mousePosition)) {
        return true;
    }

    sf::Vector2u tileCoords;
    tileCoords.x = static_cast<unsigned int>(mousePosition.x / m_scaledTileSize.x);
    tileCoords.y = static_cast<unsigned int>(mousePosition.y / m_scaledTileSize.y);

    int tileID = getTileID(tileCoords);
    if (tileID == 72 || tileID == 71) {
        return false;
    }

    return tileID > 1;
}

sf::Vector2u GameMap::getTileCoordsAt(sf::Vector2f worldPosition) const {
    if (m_scaledTileSize.x == 0.0f || m_scaledTileSize.y == 0.0f) {
        return {0,0};
    }
    auto tileX = static_cast<unsigned int>(worldPosition.x / m_scaledTileSize.x);
    auto tileY = static_cast<unsigned int>(worldPosition.y / m_scaledTileSize.y);

    return {tileX, tileY};
}

bool GameMap::isPickupOnCooldown(sf::Vector2u tileCoords) const {
    auto it = m_pickupCooldowns.find(tileCoords);
    if (it == m_pickupCooldowns.end()) {
        return false;
    }

    if (it->second.timer.getElapsedTime().asSeconds() < m_pickupCooldownDuration) {
        return true;
    }
    return false;
}

void GameMap::startPickupCooldown(sf::Vector2u tileCoords) {
    auto it = m_pickupCooldowns.find(tileCoords);

    if (it != m_pickupCooldowns.end())
    {
        it->second.timer.restart();
    }
    else
    {
        m_pickupCooldowns[tileCoords] = PickupCooldown();
    }

    std::cout<<"DEBUG Cooldown pornit pentru tile : "<<tileCoords.x<<","<<tileCoords.y<<std::endl;
}

void GameMap::updateAndDrawCooldowns(sf::RenderWindow &window) {
    for (auto it = m_pickupCooldowns.begin();it!=m_pickupCooldowns.end();) {
        float elapsed = it->second.timer.getElapsedTime().asSeconds();
        if (elapsed >= m_pickupCooldownDuration) {
            it = m_pickupCooldowns.erase(it);
        }else {
            float percent = elapsed / m_pickupCooldownDuration;
            sf::Vector2f worldPos = {
                static_cast<float>(it->first.x) * m_scaledTileSize.x,
                static_cast<float>(it->first.y) * m_scaledTileSize.y
            };
            sf::Vector2f tileCenter = getTileCenter(worldPos);
            sf::Vector2f barPos = {tileCenter.x, tileCenter.y - m_scaledTileSize.y / 2.f - 10.f};
            m_cooldownBarBg.setPosition(barPos);
            float barWidth = m_cooldownBarBg.getSize().x;
            sf::Vector2f fgPos = {barPos.x - barWidth / 2.f, barPos.y};
            m_cooldownBarFg.setPosition(fgPos);
            m_cooldownBarFg.setSize({barWidth * percent, m_cooldownBarFg.getSize().y});
            window.draw(m_cooldownBarBg);
            window.draw(m_cooldownBarFg);
            ++it;
        }
    }
}

/*
 * PENTRU METODA CU VERTEX ARRAY AM FOLOSIT https://www.sfml-dev.org/tutorials/3.0/graphics/vertex-array/#creating-an-sfml-like-entity,
 * documnetatia oficiala de pe siteul SFML,https://www.youtube.com/watch?v=hnjhCFA4GnM
 * Aceasta varianta este mai eficienta decat incarcarea in 100 spriteuri pentru a incarca o harta mare(se apelau cate 100 functii de draw
 * ceea ce era ineficient)
 * Acum mapa este incaracata complet inainte in gameLoad.
 */
