#include "GameMap.h"
#include <fstream>
#include <iostream>
#include <ostream>

GameMap::GameMap() : m_tileSize(0, 0), m_mapSize(0, 0) {}

bool GameMap::load(const std::string& mapPath, const std::string& tilesetPath,float mapScale) {
    if (!m_tilesetTexture.loadFromFile(tilesetPath)) {
        std::cerr << "EROARE: Nu am putut incarca tileset-ul: " << tilesetPath << std::endl;
        return false;
    }

    std::ifstream f(mapPath);
    if (!f.is_open()) {
        std::cerr << "EROARE: Nu am putut deschide fisierul text al harții: " << mapPath << std::endl;
        return false;
    }

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
    for (const int id : tileIDs) {
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

    const float floorTexX = 0.f;
    const float floorTexY = 0.f;

    const sf::Vector2f floorTopLeftTex(floorTexX, floorTexY);
    const sf::Vector2f floorTopRightTex(floorTexX + fTileSizeX, floorTexY);
    const sf::Vector2f floorBotRightTex(floorTexX + fTileSizeX, floorTexY + fTileSizeY);
    const sf::Vector2f floorBotLeftTex(floorTexX, floorTexY + fTileSizeY);

    for (unsigned int y = 0; y < m_mapSize.y; ++y) {
        for (unsigned int x = 0; x < m_mapSize.x; ++x) {

            //calculez pozitia pe ecran
            const sf::Vector2f topLeftPos(static_cast<float>(x) * fScaledTileSizeX, static_cast<float>(y) * fScaledTileSizeY);
            const sf::Vector2f topRightPos(topLeftPos.x + fScaledTileSizeX, topLeftPos.y);
            const sf::Vector2f botRightPos(topLeftPos.x + fScaledTileSizeX, topLeftPos.y + fScaledTileSizeY);
            const sf::Vector2f botLeftPos(topLeftPos.x, topLeftPos.y + fScaledTileSizeY);

            sf::Vertex* tileVertices = &m_vertices[vertexIndex * 6];

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
            const sf::Vector2f topLeftPos(static_cast<float>(x) * fScaledTileSizeX, static_cast<float>(y) * fScaledTileSizeY);
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

            sf::Vertex* tileVertices = &m_vertices[vertexIndex * 6];

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
    m_tileIDs = std::move(tileIDs);

    std::cout << "Harta incarcata: " << m_mapSize.x << "x" << m_mapSize.y << " piese." << std::endl;
    std::cout << "Am creat " << m_vertices.getVertexCount() << " sprite-uri pentru harta." << std::endl;
    return true;
}

void GameMap::draw(sf::RenderTarget& target,sf::RenderStates states) const{
    states.transform *= getTransform();
    states.texture = &m_tilesetTexture;
    target.draw(m_vertices, states);
}

std::ostream& operator<<(std::ostream& os, const GameMap& map) {
    os << "GameMap( Size: " << map.m_mapSize.x << "x" << map.m_mapSize.y
       << " | Tiles: " << map.m_vertices.getVertexCount() / 6 << " )";
    return os;
}

sf::FloatRect GameMap::getPixelBounds() const {
    return {{0.f,0.f},m_mapPixelSize};
}

int GameMap::getTileID(sf::Vector2u tileCoords) const {
    if (tileCoords.x >m_mapSize.x || tileCoords.y >m_mapSize.y) {
        return 0; //practic outofbons
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

    return tileID > 1;
}


/*
 * PENTRU METODA CU VERTEX ARRAY AM FOLOSIT https://www.sfml-dev.org/tutorials/3.0/graphics/vertex-array/#creating-an-sfml-like-entity,
 * documnetatia oficiala de pe siteul SFML,https://www.youtube.com/watch?v=hnjhCFA4GnM
 * Aceasta varianta este mai eficienta decat incarcarea in 100 spriteuri pentru a incarca o harta mare(se apelau cate 100 functii de draw
 * ceea ce era ineficient)
 * Acum mapa este incaracata complet inainte in gameLoad.
 */