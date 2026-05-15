#pragma once
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "Entity.h"
#include "Constants.h"
#include <vector>
#include <string>

struct Bullet;

// ============================================================
//  Player.h — игрок с анимированным спрайт-листом.
//  Разработчик А.
// ============================================================

class Player : public Entity {
public:
    Player();

    // Загрузить спрайт-лист (4 кадра × 64px = 256×64)
    bool loadTexture(const std::string& path);

    void handleInput(float dt, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const override;

    std::vector<Bullet> tryShoot(const sf::RenderWindow& window);

    void takeDamage(float amount);
    void addResource(int amount);
    void switchWeapon(int index);
    void addWeapon(Weapon w);
    void clampToMap();
    void setPosition(sf::Vector2f pos);

    bool          isAlive()       const { return health > 0.f; }
    float         getMaxHealth()  const override { return maxHealth + maxHealthBonus; }
    int           getResources()  const { return resources; }
    int           getCurrentWeaponIdx() const { return currentWeaponIdx; }
    const Weapon& getCurrentWeapon()    const { return weapons[currentWeaponIdx]; }
    sf::Vector2f  getPosition()   const override { return sprite.getPosition(); }
    float         getRadius()     const override { return C::PLAYER_RADIUS; }
    int           getWeaponCount()const { return (int)weapons.size(); }

    float speedMult      = 1.f;
    float damageMult     = 1.f;
    float maxHealthBonus = 0.f;

    float hitFlashTimer  = 0.f;   // мигание белым при уроне

private:
    sf::Texture     texture;
    sf::Sprite      sprite;
    bool            textureLoaded = false;

    // Запасная геометрия (без текстуры)
    sf::ConvexShape fallbackBody;
    sf::CircleShape fallbackDot;

    float speed;
    std::vector<Weapon> weapons;
    int currentWeaponIdx;
    int resources;

    // Анимация ходьбы (спрайт-лист 4 кадра по 64px)
    static constexpr int FRAME_W = 64;
    static constexpr int FRAME_H = 64;
    int   animFrame  = 0;
    float animTimer  = 0.f;
    float animSpeed  = 0.12f;   // секунд на кадр
    bool  isMoving   = false;

    void updateAim(const sf::RenderWindow& window);
    void drawHealthBar(sf::RenderWindow& window) const;
    void advanceAnim(float dt);
};
