#include "Weapon.h"
#include "Bullet.h"
#include "Constants.h"
#include <cmath>
#include <cstdlib>
#include <random>


//  Weapon.cpp — стрельба (прописываем логику)



static sf::Vector2f normalize(sf::Vector2f v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    return (len > 0.f) ? v / len : sf::Vector2f{1.f, 0.f};
}

// Поворачивает вектор на угол
static sf::Vector2f rotateVec(sf::Vector2f v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return { v.x * c - v.y * s, v.x * s + v.y * c };
}

//Случайный разброс при стрельбе
static float randSpread(float range) {

    return ((float)std::rand() / RAND_MAX * 2.f - 1.f) * range;
}


//Метод для создания пулек

std::vector<Bullet> Weapon::fire(sf::Vector2f pos,
                                  sf::Vector2f direction,
                                  float damageMult)
{
    std::vector<Bullet> result;
    fireCooldown = 1.f / fireRate; 

    for (int i = 0; i < bulletCount; ++i) {
        float angle       = randSpread(spread); //угол
        sf::Vector2f dir  = normalize(rotateVec(direction, angle)); //позиция
        sf::Vector2f vel  = dir * bulletSpeed; //скорость

        result.emplace_back(pos, vel, damage * damageMult, true);
    }
    return result;
}

// Характеристики пушек

Weapon Weapon::makePistol() {
    return {
        WeaponType::PISTOL,
        "Pistol",
        /*damage*/      25.f,
        /*bulletSpeed*/ 600.f,
        /*fireRate*/    2.f,
        /*bulletCount*/ 1,
        /*spread*/      0.02f
    };
}

Weapon Weapon::makeShotgun() {
    return {
        WeaponType::SHOTGUN,
        "Shotgun",
        /*damage*/      20.f, 
        /*bulletSpeed*/ 480.f,
        /*fireRate*/    1.1f,
        /*bulletCount*/ 6, 
        /*spread*/      0.28f  
    };
}

Weapon Weapon::makeMachineGun() {
    return {
        WeaponType::MACHINE_GUN,
        "Machine Gun",
        /*damage*/      11.f,
        /*bulletSpeed*/ 650.f,
        /*fireRate*/    8.f, 
        /*bulletCount*/ 1,
        /*spread*/      0.09f
    };
}
