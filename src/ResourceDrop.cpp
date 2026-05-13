#include "ResourceDrop.h"
#include "Constants.h"
#include <cmath>

// ============================================================
//  ResourceDrop.cpp — монетка с анимацией и свечением.
//  Разработчик Б.
// ============================================================

sf::Texture ResourceDrop::tex;
bool        ResourceDrop::texLoaded = false;

bool ResourceDrop::loadTexture(const std::string& path) {
    texLoaded = tex.loadFromFile(path);
    if (texLoaded) tex.setSmooth(true);
    return texLoaded;
}

ResourceDrop::ResourceDrop(sf::Vector2f pos, int amt)
    : amount(amt), collected(false)
{
    if (texLoaded) {
        sprite.setTexture(tex);
        auto sz = tex.getSize();
        sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
        sprite.setPosition(pos);
        float scale = (C::RESOURCE_RADIUS * 2.2f) / sz.x;
        sprite.setScale(scale, scale);
        sprite.setColor(sf::Color(0, 230, 190, 230));
    }
    fallback.setRadius(C::RESOURCE_RADIUS);
    fallback.setOrigin(C::RESOURCE_RADIUS, C::RESOURCE_RADIUS);
    fallback.setPosition(pos);
    fallback.setFillColor(sf::Color(0, 220, 170, 220));
    fallback.setOutlineThickness(1.5f);
    fallback.setOutlineColor(sf::Color(100, 255, 230));
}

void ResourceDrop::update(float dt) {
    bobTimer += dt;
    // Синусоидальное покачивание по Y
    float bob = std::sin(bobTimer * 3.5f) * 1.5f;
    sf::Vector2f base = texLoaded ? sprite.getPosition() : fallback.getPosition();
    // Применяем только визуальный сдвиг через цвет/масштаб
    if (texLoaded) {
        // Вращение монетки
        sprite.rotate(90.f * dt);
        // Пульсация яркости
        float pulse = 0.85f + 0.15f * std::sin(bobTimer * 4.f);
        sf::Uint8 bright = (sf::Uint8)(pulse * 230.f);
        sprite.setColor(sf::Color(0, bright, (sf::Uint8)(bright * 0.83f), 220));
    }
}

sf::Vector2f ResourceDrop::getPosition() const {
    return texLoaded ? sprite.getPosition() : fallback.getPosition();
}

void ResourceDrop::draw(sf::RenderWindow& window) const {
    if (collected) return;

    sf::RenderStates glow;
    glow.blendMode = sf::BlendAdd;

    if (texLoaded) {
        // Ореол свечения
        sf::Sprite halo = sprite;
        halo.setScale(sprite.getScale() * 2.0f);
        halo.setColor(sf::Color(0, 180, 140, 50));
        window.draw(halo, glow);
        window.draw(sprite, glow);
    } else {
        window.draw(fallback);
    }
}
