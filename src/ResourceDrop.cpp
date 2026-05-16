#include "ResourceDrop.h"
#include <cmath>

// ============================================================
//  ResourceDrop.cpp
//  Разработчик Б.
// ============================================================

sf::Texture ResourceDrop::texMoney;
sf::Texture ResourceDrop::texAmmo;
bool        ResourceDrop::texLoaded = false;

bool ResourceDrop::loadTextures(const std::string& moneyPath,
                                 const std::string& ammoPath)
{
    bool ok = texMoney.loadFromFile(moneyPath) &&
               texAmmo.loadFromFile(ammoPath);
    texMoney.setSmooth(true);
    texAmmo.setSmooth(true);
    texLoaded = ok;
    return ok;
}

ResourceDrop::ResourceDrop(sf::Vector2f pos, int amt, DropType type)
    : dropType(type), amount(amt), collected(false)
{
    // Выбрать текстуру по типу
    sf::Texture& tex = (type == DropType::MONEY) ? texMoney : texAmmo;

    if (texLoaded) {
        sprite.setTexture(tex);
        auto sz = tex.getSize();
        sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
        sprite.setPosition(pos);
        float scale = (C::RESOURCE_RADIUS * 2.4f) / sz.x;
        sprite.setScale(scale, scale);
        // Монетка — зелёная, патрон — жёлтая
        sprite.setColor(type == DropType::MONEY
                        ? sf::Color(0, 230, 180, 220)
                        : sf::Color(255, 220, 60, 220));
    }

    float r = C::RESOURCE_RADIUS;
    fallback.setRadius(r);
    fallback.setOrigin(r, r);
    fallback.setPosition(pos);
    fallback.setFillColor(type == DropType::MONEY
                          ? sf::Color(0, 200, 150, 220)
                          : sf::Color(240, 200, 0, 220));
    fallback.setOutlineThickness(1.5f);
    fallback.setOutlineColor(type == DropType::MONEY
                             ? sf::Color(80, 255, 200)
                             : sf::Color(255, 240, 100));
}

void ResourceDrop::update(float dt) {
    bobTimer += dt;
    if (texLoaded) {
        sprite.rotate(80.f * dt);
        float pulse = 0.85f + 0.15f * std::sin(bobTimer * 4.f);
        sf::Color c = sprite.getColor();
        c.a = (sf::Uint8)(pulse * 220.f);
        sprite.setColor(c);
    }
}

sf::Vector2f ResourceDrop::getPosition() const {
    return texLoaded ? sprite.getPosition() : fallback.getPosition();
}

void ResourceDrop::draw(sf::RenderWindow& window) const {
    if (collected) return;
    if (texLoaded) {
        sf::RenderStates glow;
        glow.blendMode = sf::BlendAdd;
        sf::Sprite halo = sprite;
        halo.setScale(sprite.getScale() * 2.0f);
        sf::Color hc = sprite.getColor(); hc.a = 40;
        halo.setColor(hc);
        window.draw(halo, glow);
        window.draw(sprite, glow);
    } else {
        window.draw(fallback);
    }
}
