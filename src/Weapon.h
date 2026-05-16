#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// ============================================================
//  Weapon.h — оружие с обоймой, перезарядкой и уровнями.
//  Разработчик Б.
// ============================================================

struct Bullet;

enum class WeaponType { PISTOL, SHOTGUN, MACHINE_GUN };

struct Weapon {
    WeaponType  type;
    std::string name;

    // ── Базовые характеристики (меняются апгрейдами) ──────
    float  damage;
    float  bulletSpeed;
    float  fireRate;      // выстрелов/сек
    int    bulletCount;
    float  spread;

    // ── Обойма ────────────────────────────────────────────
    int    magazineSize;  // максимум пуль в обойме
    int    currentAmmo;   // сейчас в обойме
    int    totalAmmo;     // запас пуль (не считая обойму)
    float  reloadTime;    // секунд на перезарядку

    // ── Состояние (считается в реальном времени) ──────────
    float  fireCooldown = 0.f;
    bool   reloading    = false;
    float  reloadTimer  = 0.f;

    // ── Уровни апгрейдов ──────────────────────────────────
    // Каждый уровень 0..3, 0 = не куплен
    int    damageLevel   = 0;  // +20% урона за уровень
    int    fireRateLevel = 0;  // +15% скорострельности за уровень
    int    magLevel      = 0;  // +25% обоймы за уровень

    // ── Методы ────────────────────────────────────────────
    bool canFire() const {
        return fireCooldown <= 0.f && currentAmmo > 0 && !reloading;
    }
    bool needsReload() const {
        return currentAmmo == 0 && !reloading && totalAmmo > 0;
    }

    void updateCooldown(float dt);
    void startReload();
    void addAmmo(int amount);   // добавить патроны в запас

    // Создать пули (уменьшает currentAmmo на bulletCount)
    std::vector<Bullet> fire(sf::Vector2f pos,
                             sf::Vector2f direction,
                             float damageMult = 1.f);

    // Реальная скорострельность с учётом апгрейда
    float effectiveFireRate()  const { return fireRate * (1.f + fireRateLevel * 0.15f); }
    float effectiveDamage()    const { return damage   * (1.f + damageLevel   * 0.20f); }
    int   effectiveMagSize()   const;

    // Стоимость следующего апгрейда урона/скорострельности/обоймы
    int   damageCost()   const;
    int   fireRateCost() const;
    int   magCost()      const;

    // Фабрики
    static Weapon makePistol();
    static Weapon makeShotgun();
    static Weapon makeMachineGun();
};
