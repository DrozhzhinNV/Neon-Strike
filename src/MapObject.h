#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ============================================================
//  MapObject.h — статический объект на карте
//  (дерево, здание, машина, баррикада).
//
//  Эти объекты:
//   - рисуются поверх земли
//   - НЕ двигаются
//   - могут блокировать путь врагам (поле solid)
//  Разработчик Б.
// ============================================================

enum class MapObjType {
    TREE,
    BUILDING_SMALL,
    BUILDING_LARGE,
    CAR,
    BARRIER
};

struct MapObject {
    MapObjType   type;
    sf::Sprite   sprite;
    sf::FloatRect  bounds;   // прямоугольник коллизии (для врагов / игрока)
    bool         solid;      // true = нельзя пройти сквозь

    MapObject(MapObjType t,
              sf::Vector2f pos,
              sf::Texture& tex,
              float scale = 1.f,
              bool  isSolid = false);

    void draw(sf::RenderWindow& window) const;

    // Обновить bounds после перемещения
    void updateBounds();
};
