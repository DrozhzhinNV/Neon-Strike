#pragma once
#include <SFML/Graphics.hpp>


//  Enemy.h — один враг.


enum class EnemyType {
    BASIC, // обычный
    FAST,  // быстрый
    TANK   // толстый
};

class Enemy {
public:
    Enemy(EnemyType type, sf::Vector2f startPos);


    void update(float dt, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window) const;

    bool takeDamage(float amount);

    bool         isAlive()    const { return health > 0.f; }
    float        getDamage()  const { return contactDamage; }
    int          getReward()  const { return reward; }
    EnemyType    getType()    const { return type; }
    sf::Vector2f getPosition()const { return body.getPosition(); }
    float        getRadius()  const { return body.getRadius(); }

    //Задержка контактного урона
    float damageCooldown = 0.f;

private:
    EnemyType       type;
    sf::CircleShape body;
    float           health;
    float           maxHealth;
    float           speed;
    float           contactDamage;
    int             reward;

    void drawHealthBar(sf::RenderWindow& window) const;
};
