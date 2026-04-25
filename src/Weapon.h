#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

//  Weapon.h — описание оружия.


struct Bullet; 

enum class WeaponType {
    PISTOL,      // Пистолет
    SHOTGUN,     // Дробовик
    MACHINE_GUN  // Пулемёт
};

struct Weapon {
    WeaponType  type;
    std::string name;
    float       damage;      
    float       bulletSpeed;  
    float       fireRate;     
    int         bulletCount;  
    float       spread;     

    float fireCooldown = 0.f;  //Задержка стрельбы

    bool canFire() const { return fireCooldown <= 0.f; }


    void updateCooldown(float dt) {
        if (fireCooldown > 0.f) fireCooldown -= dt;
    }
    

    //Генерация пулек
    std::vector<Bullet> fire(sf::Vector2f pos,
                             sf::Vector2f direction,
                             float damageMult = 1.f);


    static Weapon makePistol();
    static Weapon makeShotgun();
    static Weapon makeMachineGun();
};
