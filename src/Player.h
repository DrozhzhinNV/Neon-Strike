#pragma once
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "Entity.h"
#include <vector>
#include "Constants.h"

struct Bullet;

class Player : public Entity {
public:
    Player();

    // Загрузить текстуру спрайта
    bool loadTexture(const std::string& path);

    void handleInput(float dt, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const override;

    std::vector<Bullet> tryShoot(const sf::RenderWindow& window);

    void takeDamage(float amount);

    void addResource(int amount);
    void switchWeapon(int index);
    void addWeapon(Weapon w);

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

    void clampToMap();
    void setPosition(sf::Vector2f pos) { sprite.setPosition(pos); fallbackBody.setPosition(pos); }

    // Вспышка при получении урона (таймер белой подсветки)
    float hitFlashTimer = 0.f;


private:
    sf::Texture     texture;
    sf::Sprite      sprite;
    bool            textureLoaded = false;

    // Запасная геометрия на случай отсутствия текстуры
    sf::ConvexShape fallbackBody;
    sf::CircleShape fallbackDot;

    float speed;

    std::vector<Weapon> weapons;
    int currentWeaponIdx;
    int resources;

    void updateAim(const sf::RenderWindow& window);
    void drawHealthBar(sf::RenderWindow& window) const;
};
