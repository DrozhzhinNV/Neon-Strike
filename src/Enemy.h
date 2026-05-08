#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"

enum class EnemyType {
    BASIC, FAST, TANK
};

class Enemy : public Entity {
public:
    Enemy(EnemyType type, sf::Vector2f startPos);

    void update(float dt, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window) const override;

    bool takeDamage(float amount);  

    bool         isAlive()    const { return health > 0.f; } 
    float        getDamage()  const { return contactDamage; }
    int          getReward()  const { return reward; }
    EnemyType    getType()    const { return type; }
    sf::Vector2f getPosition()const override { return body.getPosition(); }
    float        getRadius()  const override { return body.getRadius(); }

    float damageCooldown = 0.f;

private:
    EnemyType       type;
    sf::CircleShape body;
    float           speed;
    float           contactDamage;
    int             reward;

    void drawHealthBar(sf::RenderWindow& window) const;
};