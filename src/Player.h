#pragma once
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "Entity.h"
#include "Constants.h"
#include <vector>
#include <string>

struct Bullet;

// ============================================================
//  Player.h — игрок с анимацией, обоймой и перезарядкой.
//  Разработчик А.
// ============================================================

class Player : public Entity {
public:
    Player();

    bool loadTexture(const std::string& path);

    void handleInput(float dt, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const override;
    std::vector<Bullet> tryShoot(const sf::RenderWindow& window);

    void takeDamage(float amount);
    void addResource(int amount);
    void addAmmo(int amount);        // добавить патроны в текущее оружие
    void switchWeapon(int index);
    void addWeapon(Weapon w);
    void clampToMap();
    void setPosition(sf::Vector2f pos);
    void startReload();              // ручная перезарядка (R)

    bool          isAlive()       const { return health > 0.f; }
    float         getMaxHealth()  const override { return maxHealth + maxHealthBonus; }
    int           getResources()  const { return resources; }
    int           getCurrentWeaponIdx()  const { return currentWeaponIdx; }
    Weapon&       getCurrentWeapon()           { return weapons[currentWeaponIdx]; }
    const Weapon& getCurrentWeapon()     const { return weapons[currentWeaponIdx]; }
    sf::Vector2f  getPosition()   const override { return sprite.getPosition(); }
    float         getRadius()     const override { return C::PLAYER_RADIUS; }
    int           getWeaponCount()const { return (int)weapons.size(); }
    std::vector<Weapon>& getWeapons();
    const std::vector<Weapon>& getWeapons() const;

    float speedMult      = 1.f;
    float damageMult     = 1.f;
    float maxHealthBonus = 0.f;
    float hitFlashTimer  = 0.f;

private:
    sf::Texture     texture;
    sf::Sprite      sprite;
    bool            textureLoaded = false;

    sf::ConvexShape fallbackBody;
    sf::CircleShape fallbackDot;

    float speed;
    std::vector<Weapon> weapons;
    int currentWeaponIdx;
    int resources;

    // Анимация (спрайт-лист 4 кадра × 64 пикс)
    static constexpr int FRAME_W = 64;
    static constexpr int FRAME_H = 64;
    int   animFrame = 0;
    float animTimer = 0.f;
    float animSpeed = 0.12f;
    bool  isMoving  = false;

    void updateAim(const sf::RenderWindow& window);
    void drawHealthBar(sf::RenderWindow& window) const;
    void advanceAnim(float dt);
};
