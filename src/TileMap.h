#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "MapObject.h"

// ============================================================
//  TileMap.h — тайловая карта + расстановка объектов.
//
//  Карта делится на зоны:
//   - центр (400x400) — открытое поле (асфальт), игрок стартует здесь
//   - периметр — трава + здания + деревья
//   - дороги   — асфальтовые полосы крест-накрест
//
//  Разработчик Б.
// ============================================================

class TileMap : public sf::Drawable {
public:
    TileMap();

    // Загрузить текстуры тайлов и объектов. Вызвать один раз.
    bool loadTextures();

    // Сгенерировать карту. seed — для воспроизводимости.
    void generate(unsigned int seed = 42);

    // Рисовать тайлы (земля)
    void drawGround(sf::RenderWindow& window) const;

    // Рисовать объекты поверх земли (деревья, здания…)
    void drawObjects(sf::RenderWindow& window) const;

    // Список объектов (Game.cpp проверяет коллизии)
    const std::vector<MapObject>& getObjects() const { return objects; }

private:
    // Размер одного тайла в пикселях
    static constexpr int TILE = 64;

    // Количество тайлов по X и Y
    int tilesX, tilesY;

    // Двумерный массив: 0=асфальт 1=трава
    std::vector<std::vector<int>> grid;

    // Тайловые спрайты (один sf::VertexArray — быстро)
    sf::VertexArray   groundVerts;
    sf::Texture       texGround;   // tile_ground.png
    sf::Texture       texGrass;    // tile_grass.png

    // Объекты карты и их текстуры
    std::vector<MapObject> objects;
    sf::Texture texTree;
    sf::Texture texBuildSmall;
    sf::Texture texBuildLarge;
    sf::Texture texCar;
    sf::Texture texBarrier;

    // sf::Drawable interface (не используем напрямую)
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}

    void buildVertexArray();
    void placeObjects(unsigned int seed);
    bool overlapsExisting(sf::FloatRect r, float margin = 16.f) const;
};
