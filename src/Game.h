#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "ResourceDrop.h"
#include "WaveManager.h"
#include "UpgradeSystem.h"
#include "HUD.h"
#include "SaveSystem.h"
#include "Particle.h"
#include "TileMap.h"
#include "WeaponShop.h"
#include "Weather.h"
#include <vector>
#include <string>

// ============================================================
//  Game.h
//  Разработчик А.
// ============================================================

enum class GameState {
    PLAYING,
    PAUSED_SHOP,   // пауза — открыт магазин оружия
    WAVE_CLEAR,
    UPGRADE_MENU,
    GAME_OVER
};

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::View         camera;

    Player        player;
    WaveManager   waveManager;
    HUD           hud;
    UpgradeSystem upgradeSystem;
    WeaponShop    weaponShop;
    ParticleSystem particles;
    TileMap        tileMap;

    std::vector<Enemy>        enemies;
    std::vector<Bullet>       bullets;
    std::vector<ResourceDrop> drops;

    GameState gameState;
    int       score;
    float     waveClearTimer;

    sf::RectangleShape mapBackground;
    sf::RectangleShape mapBorder;

    SaveData  savedData;
    sf::Font  overlayFont;
    bool      overlayFontLoaded;

    // Босс
    bool  bossAlive      = false;
    bool  bossWaveActive = false; // true пока идёт волна босса
    float bossMaxHP     = 0.f;
    float bossCurrentHP = 0.f;
    float bossKillMsgTimer = 0.f;

    // Tab — состояние клавиши (edge-detect)
    bool tabWasPressed = false;

    void processEvents();
    void update(float dt);
    void render();

    void updateBullets(float dt);
    void updateEnemies(float dt);
    void checkCollisions();
    void collectDrops();
    void removeDeadObjects();
    void applyCamera();
    void saveGame();
    void applySaveData(const SaveData& d);
    void drawOverlay(const std::string& msg, sf::Color color);
    void loadAllTextures();
    void spawnEnemyWithTexture(Enemy& e);
    sf::Vector2f resolveMapCollision(sf::Vector2f pos, float radius);
    void drawBossBar();

    // Выпасть монеткам и патронам из врага
    void spawnDrops(const Enemy& enemy);

    Weather weather;
};
