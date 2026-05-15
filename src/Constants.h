#pragma once

// Constants.h — все числовые константы игры в одном месте.

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

    // Босс — появляется каждые 5 волн
    constexpr float BOSS_RADIUS     = 50.f;
    constexpr float BOSS_SPEED      = 38.f;
    constexpr float BOSS_HP         = 1200.f;
    constexpr float BOSS_DAMAGE     = 55.f;
    constexpr int   BOSS_REWARD     = 200;
    constexpr float BOSS_HP_REWARD  = 50.f;  // HP игроку за убийство босса
    constexpr int   BOSS_WAVE_STEP  = 5;     // каждые 5 волн

    constexpr float RESOURCE_RADIUS = 8.f;
}
