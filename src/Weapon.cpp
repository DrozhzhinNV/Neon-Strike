#include "Weapon.h"
#include "Bullet.h"
#include "Constants.h"
#include <cmath>
#include <cstdlib>

// ============================================================
//  Weapon.cpp
//  Разработчик Б.
// ============================================================

static sf::Vector2f normalize(sf::Vector2f v) {
    float len = std::sqrt(v.x*v.x + v.y*v.y);
    return (len > 0.f) ? v/len : sf::Vector2f{1.f,0.f};
}
static sf::Vector2f rotateVec(sf::Vector2f v, float a) {
    float c=std::cos(a), s=std::sin(a);
    return {v.x*c-v.y*s, v.x*s+v.y*c};
}
static float randSpread(float r) {
    return ((float)std::rand()/RAND_MAX*2.f-1.f)*r;
}

// ── Реальный размер обоймы с апгрейдом ───────────────────────
int Weapon::effectiveMagSize() const {
    return (int)(magazineSize * (1.f + magLevel * 0.25f));
}

// ── Стоимость следующего апгрейда ────────────────────────────
static int upgradeCost(int level) {
    if (level >= C::SHOP_MAX_LEVEL) return 0;
    switch (level) {
        case 0: return C::SHOP_COST_1;
        case 1: return C::SHOP_COST_2;
        default:return C::SHOP_COST_3;
    }
}
int Weapon::damageCost()   const { return upgradeCost(damageLevel);   }
int Weapon::fireRateCost() const { return upgradeCost(fireRateLevel); }
int Weapon::magCost()      const { return upgradeCost(magLevel);      }

// ── Обновление таймеров ───────────────────────────────────────
void Weapon::updateCooldown(float dt) {
    if (fireCooldown > 0.f) fireCooldown -= dt;

    if (reloading) {
        reloadTimer -= dt;
        if (reloadTimer <= 0.f) {
            // Перезарядка завершена: заполнить обойму из запаса
            int need  = effectiveMagSize() - currentAmmo;
            int take  = std::min(need, totalAmmo);
            currentAmmo += take;
            totalAmmo   -= take;
            reloading    = false;
        }
    }

    // Авто-перезарядка если обойма пустая
    if (currentAmmo == 0 && !reloading && totalAmmo > 0)
        startReload();
}

void Weapon::startReload() {
    if (reloading) return;
    if (totalAmmo <= 0) return;
    if (currentAmmo >= effectiveMagSize()) return;
    reloading    = true;
    reloadTimer  = reloadTime;
    fireCooldown = 0.f;
}

void Weapon::addAmmo(int amount) {
    totalAmmo += amount;
}

// ── Выстрел ──────────────────────────────────────────────────
std::vector<Bullet> Weapon::fire(sf::Vector2f pos,
                                  sf::Vector2f direction,
                                  float damageMult)
{
    std::vector<Bullet> result;
    fireCooldown = 1.f / effectiveFireRate();

    // Сколько реально выпустим (не больше чем в обойме)
    int shots = std::min(bulletCount, currentAmmo);
    currentAmmo -= shots;

    for (int i = 0; i < shots; ++i) {
        float angle      = randSpread(spread);
        sf::Vector2f dir = normalize(rotateVec(direction, angle));
        sf::Vector2f vel = dir * bulletSpeed;
        result.emplace_back(pos, vel, effectiveDamage() * damageMult, true);
    }
    return result;
}

// ── Фабрики ──────────────────────────────────────────────────
Weapon Weapon::makePistol() {
    Weapon w;
    w.type        = WeaponType::PISTOL;
    w.name        = "Pistol";
    w.damage      = 25.f;
    w.bulletSpeed = 600.f;
    w.fireRate    = 2.f;
    w.bulletCount = 1;
    w.spread      = 0.02f;
    w.magazineSize= C::PISTOL_MAG;
    w.currentAmmo = C::PISTOL_MAG;
    w.totalAmmo   = C::PISTOL_TOTAL;
    w.reloadTime  = C::PISTOL_RELOAD;
    return w;
}
Weapon Weapon::makeShotgun() {
    Weapon w;
    w.type        = WeaponType::SHOTGUN;
    w.name        = "Shotgun";
    w.damage      = 20.f;
    w.bulletSpeed = 480.f;
    w.fireRate    = 1.1f;
    w.bulletCount = 6;
    w.spread      = 0.28f;
    w.magazineSize= C::SHOTGUN_MAG;
    w.currentAmmo = C::SHOTGUN_MAG;
    w.totalAmmo   = C::SHOTGUN_TOTAL;
    w.reloadTime  = C::SHOTGUN_RELOAD;
    return w;
}
Weapon Weapon::makeMachineGun() {
    Weapon w;
    w.type        = WeaponType::MACHINE_GUN;
    w.name        = "MachineGun";
    w.damage      = 11.f;
    w.bulletSpeed = 650.f;
    w.fireRate    = 8.f;
    w.bulletCount = 1;
    w.spread      = 0.09f;
    w.magazineSize= C::MG_MAG;
    w.currentAmmo = C::MG_MAG;
    w.totalAmmo   = C::MG_TOTAL;
    w.reloadTime  = C::MG_RELOAD;
    return w;
}
