#pragma once
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include <vector>
#include "Constants.h"

//  Player.h — класс игрока.


struct Bullet;

class Player {
public:
    Player();

    // Обработать клавиатуру + вращение к мыши
    void handleInput(float dt, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;

    // Попытаться выстрелить. Возвращает пустой вектор если нельзя.
    std::vector<Bullet> tryShoot(const sf::RenderWindow& window);

    void takeDamage(float amount);
    void heal(float amount);
    void addResource(int amount);
    void switchWeapon(int index);
    void addWeapon(Weapon w);           // добавить новое оружие (не дубликат)

    // --- Геттеры ---
    bool         isAlive()      const { return health > 0.f; }
    float        getHealth()    const { return health; }
    float        getMaxHealth() const { return maxHealth + maxHealthBonus; }
    int          getResources() const { return resources; }
    int          getCurrentWeaponIdx() const { return currentWeaponIdx; }
    const Weapon& getCurrentWeapon()  const { return weapons[currentWeaponIdx]; }
    sf::Vector2f getPosition()  const { return body.getPosition(); }
    float        getRadius()    const { return C::PLAYER_RADIUS; }
    int          getWeaponCount()const{ return (int)weapons.size(); }

    // --- Модификаторы от апгрейдов (публичные — UpgradeSystem меняет напрямую) ---
    float speedMult     = 1.f;   // множитель скорости
    float damageMult    = 1.f;   // множитель урона
    float maxHealthBonus= 0.f;   // бонус к макс. HP

    void clampToMap();

private:
    sf::ConvexShape body;    // треугольник — тело игрока
    sf::CircleShape gunDot;  // маленький кружок на «стволе»

    float health;
    float maxHealth;
    float speed;

    std::vector<Weapon> weapons;
    int currentWeaponIdx;
    int resources;

    void updateAim(const sf::RenderWindow& window);
    void drawHealthBar(sf::RenderWindow& window) const;
};
