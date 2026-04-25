#pragma once
#include <SFML/Graphics.hpp>

//  Bullet.h — одна пуля.


struct Bullet {
    sf::CircleShape shape;
    sf::Vector2f    velocity;
    float           damage;
    bool            active;
    bool            fromPlayer; // true = пуля игрока, false = врага

    Bullet(sf::Vector2f pos, sf::Vector2f vel, float dmg, bool player);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    sf::Vector2f getCenter() const { return shape.getPosition(); }
    float        getRadius() const { return shape.getRadius(); }
};
