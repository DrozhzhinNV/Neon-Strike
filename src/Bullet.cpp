#include "Bullet.h"
#include "Constants.h"
#include <cmath>

// ============================================================
//  Bullet.cpp — реализация пули.
//  Разработчик А.
// ============================================================

// Статические члены
sf::Texture Bullet::playerTex;
bool        Bullet::playerTexLoaded = false;

bool Bullet::loadPlayerTexture(const std::string& path) {
    playerTexLoaded = playerTex.loadFromFile(path);
    if (playerTexLoaded) playerTex.setSmooth(true);
    return playerTexLoaded;
}

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f vel, float dmg, bool player)
    : velocity(vel), damage(dmg), active(true), fromPlayer(player)
{
    // Направление скорости → угол поворота спрайта
    float angle = std::atan2(vel.y, vel.x) * 180.f / 3.14159f;

    if (player && playerTexLoaded) {
        sprite.setTexture(playerTex);
        auto sz = playerTex.getSize();
        sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
        sprite.setPosition(pos);
        sprite.setRotation(angle);
        sprite.setScale(0.7f, 0.7f);
        sprite.setColor(sf::Color(255, 230, 80, 240));
    }

    // Запасная форма
    fallback.setRadius(C::BULLET_RADIUS);
    fallback.setOrigin(C::BULLET_RADIUS, C::BULLET_RADIUS);
    fallback.setPosition(pos);
    fallback.setFillColor(player ? sf::Color(255, 220, 0)
                                 : sf::Color(255, 80, 80));
}

void Bullet::update(float dt) {
    sf::Vector2f delta = velocity * dt;
    sprite.move(delta);
    fallback.move(delta);

    sf::Vector2f p = fromPlayer ? sprite.getPosition() : fallback.getPosition();
    if (p.x < -100.f || p.x > C::MAP_W + 100.f ||
        p.y < -100.f || p.y > C::MAP_H + 100.f)
        active = false;
}

sf::Vector2f Bullet::getCenter() const {
    return (fromPlayer && playerTexLoaded)
        ? sprite.getPosition()
        : fallback.getPosition();
}

void Bullet::draw(sf::RenderWindow& window) const {
    if (!active) return;

    if (fromPlayer && playerTexLoaded) {
        // Аддитивный режим — пуля светится
        sf::RenderStates st;
        st.blendMode = sf::BlendAdd;
        // Ореол
        sf::Sprite halo = sprite;
        halo.setScale(1.8f, 1.8f);
        halo.setColor(sf::Color(255, 180, 0, 80));
        window.draw(halo, st);
        window.draw(sprite, st);
    } else {
        window.draw(fallback);
    }
}
