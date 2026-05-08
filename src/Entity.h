#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    Entity(float maxHP) : health(maxHP), maxHealth(maxHP) {}
    virtual ~Entity() = default;

    bool isAlive() const { return health > 0.f; }
    float getHealth() const { return health; }
    virtual float getMaxHealth() const { return maxHealth; }

    void heal(float amount) {
        health = std::min(health + amount, getMaxHealth());
    }

    virtual sf::Vector2f getPosition() const = 0;
    virtual float getRadius() const = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;

protected:
    float health;
    float maxHealth;

    void applyDamage(float amount) {
        health -= amount;
        if (health < 0.f) health = 0.f;
    }
};