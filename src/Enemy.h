#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include <string>

// ============================================================
//  Enemy.h
//  Разработчик Б.
//
//  ВАЖНО: sf::Sprite хранит УКАЗАТЕЛЬ на sf::Texture.
//  При копировании объекта Enemy стандартным copy constructor-ом
//  sprite будет указывать на texture СТАРОГО объекта.
//  Поэтому мы определяем свой copy constructor и operator=,
//  которые после копирования перепривязывают sprite к новой texture.
// ============================================================

enum class EnemyType { BASIC, FAST, TANK, BOSS };

class Enemy : public Entity {
public:
    Enemy(EnemyType type, sf::Vector2f startPos);

    // ── Кастомные copy/move — перепривязывают sprite к texture ──
    Enemy(const Enemy& other);
    Enemy& operator=(const Enemy& other);
    Enemy(Enemy&& other) noexcept;
    Enemy& operator=(Enemy&& other) noexcept;

    // Загрузить спрайт-лист. frameW/frameH — размер ОДНОГО кадра.
    bool loadTexture(const std::string& path, int frameW, int frameH);

    void update(float dt, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window) const override;
    bool takeDamage(float amount);

    bool         isAlive()     const { return health > 0.f; }
    float        getDamage()   const { return contactDamage; }
    int          getReward()   const { return reward; }
    EnemyType    getType()     const { return type; }
    sf::Vector2f getPosition() const override { return body.getPosition(); }
    float        getRadius()   const override { return body.getRadius(); }
    bool         isBoss()      const { return type == EnemyType::BOSS; }
    float        getHealth()   const { return health; }

    void pushOut(sf::Vector2f delta) {
        body.move(delta);
        if (textureLoaded) sprite.move(delta);
    }

    sf::Color typeColor() const;

    float damageCooldown = 0.f;

private:
    EnemyType       type;
    sf::CircleShape body;
    sf::Texture     texture;
    sf::Sprite      sprite;
    bool            textureLoaded = false;

    int   frameW = 0, frameH = 0;
    int   animFrame  = 0;
    float animTimer  = 0.f;
    float animSpeed  = 0.12f;
    float aimAngle   = 0.f;
    float speed;
    float contactDamage;
    int   reward;

    // После копирования texture → нужно обновить указатель sprite
    void rebindSprite();

    void advanceAnim(float dt, bool moving);
    void drawHealthBar(sf::RenderWindow& window) const;
};
