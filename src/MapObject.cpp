#include "MapObject.h"

// ============================================================
//  MapObject.cpp
//  Разработчик Б.
// ============================================================

MapObject::MapObject(MapObjType t, sf::Vector2f pos,
                     sf::Texture& tex, float scale, bool isSolid)
    : type(t), solid(isSolid)
{
    sprite.setTexture(tex);
    tex.setSmooth(true);

    // Центрировать origin на середине спрайта
    auto sz = tex.getSize();
    sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
    sprite.setPosition(pos);
    sprite.setScale(scale, scale);

    updateBounds();
}

void MapObject::updateBounds() {
    // bounds — центральная часть спрайта (70% от размера)
    // чтобы можно было чуть зайти за угол объекта
    sf::FloatRect full = sprite.getGlobalBounds();
    float shrink = 0.30f;
    float sw = full.width  * shrink;
    float sh = full.height * shrink;
    bounds = sf::FloatRect(
        full.left   + sw / 2.f,
        full.top    + sh / 2.f,
        full.width  - sw,
        full.height - sh
    );
}

void MapObject::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}
