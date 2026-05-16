#pragma once

namespace C {
    constexpr int   WINDOW_W  = 1024;
    constexpr int   WINDOW_H  = 768;
    constexpr int   FPS_LIMIT = 60;

    constexpr float MAP_W = 2000.f;
    constexpr float MAP_H = 1600.f;
    constexpr float PI    = 3.14159265358979f;

    constexpr float PLAYER_SPEED    = 220.f;
    constexpr float PLAYER_MAX_HP   = 100.f;
    constexpr float PLAYER_RADIUS   = 16.f;
    constexpr float BULLET_RADIUS   = 4.f;

    // Размеры спрайтов — множитель от радиуса
    constexpr float PLAYER_SPRITE_SCALE = 5.5f;
    constexpr float ENEMY_SPRITE_SCALE  = 5.0f;

    // ── Враги ─────────────────────────────────────────
    constexpr float BASIC_RADIUS    = 18.f;
    constexpr float BASIC_SPEED     = 90.f;
    constexpr float BASIC_HP        = 50.f;
    constexpr float BASIC_DAMAGE    = 15.f;
    constexpr int   BASIC_REWARD    = 10;

    constexpr float FAST_RADIUS     = 12.f;
    constexpr float FAST_SPEED      = 170.f;
    constexpr float FAST_HP         = 25.f;
    constexpr float FAST_DAMAGE     = 8.f;
    constexpr int   FAST_REWARD     = 7;

    constexpr float TANK_RADIUS     = 30.f;
    constexpr float TANK_SPEED      = 50.f;
    constexpr float TANK_HP         = 250.f;
    constexpr float TANK_DAMAGE     = 35.f;
    constexpr int   TANK_REWARD     = 40;

    constexpr float BOSS_RADIUS     = 50.f;
    constexpr float BOSS_SPEED      = 38.f;
    constexpr float BOSS_HP         = 1200.f;
    constexpr float BOSS_DAMAGE     = 55.f;
    constexpr int   BOSS_REWARD     = 200;
    constexpr float BOSS_HP_REWARD  = 50.f;
    constexpr int   BOSS_WAVE_STEP  = 5;

    constexpr float RESOURCE_RADIUS = 8.f;

    // ── Обоймы и перезарядка ─────────────────────────
    constexpr int   PISTOL_MAG      = 12;
    constexpr int   PISTOL_TOTAL    = 60;
    constexpr float PISTOL_RELOAD   = 1.4f;

    constexpr int   SHOTGUN_MAG     = 6;
    constexpr int   SHOTGUN_TOTAL   = 30;
    constexpr float SHOTGUN_RELOAD  = 2.0f;

    constexpr int   MG_MAG          = 30;
    constexpr int   MG_TOTAL        = 150;
    constexpr float MG_RELOAD       = 2.5f;

    // ── Магазин оружия ───────────────────────────────
    constexpr int   SHOP_COST_1     = 40;
    constexpr int   SHOP_COST_2     = 80;
    constexpr int   SHOP_COST_3     = 130;
    constexpr int   SHOP_MAX_LEVEL  = 3;

} // ← ОБЯЗАТЕЛЬНО: закрываем namespace C