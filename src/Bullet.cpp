#include "Bullet.h"
#include "Constants.h"

//  Bullet.cpp — реализация пули.


Bullet::Bullet(sf::Vector2f pos, sf::Vector2f vel, float dmg, bool player)
    : velocity(vel), damage(dmg), active(true), fromPlayer(player)
{
    shape.setRadius(C::BULLET_RADIUS);
    shape.setOrigin(C::BULLET_RADIUS, C::BULLET_RADIUS);
    shape.setPosition(pos);

    // Пули игрока — жёлтые, врагов — красные
    shape.setFillColor(player ? sf::Color(255, 220, 0)
                              : sf::Color(255, 80,  80));
}

void Bullet::update(float dt) {
    shape.move(velocity * dt);

    // Деактивировать пулю, если она вылетела за карту
    sf::Vector2f p = shape.getPosition();
    if (p.x < -100.f || p.x > C::MAP_W + 100.f ||
        p.y < -100.f || p.y > C::MAP_H + 100.f)
    {
        active = false;
    }
}

void Bullet::draw(sf::RenderWindow& window) const {
    if (active) window.draw(shape);
}
