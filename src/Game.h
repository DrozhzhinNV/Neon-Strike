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
#include <vector>
#include <string>

// ============================================================
//  Game.h — центральный класс.
//  Разработчик А.
// ============================================================

enum class GameState { PLAYING, WAVE_CLEAR, UPGRADE_MENU, GAME_OVER };

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
    ParticleSystem particles;
    TileMap        tileMap;

    std::vector<Enemy>        enemies;
    std::vector<Bullet>       bullets;
    std::vector<ResourceDrop> drops;

    GameState gameState;
    int       score;
    float     waveClearTimer;

    // Запасной фон
    sf::RectangleShape mapBackground;
    sf::RectangleShape mapBorder;

    SaveData  savedData;
    sf::Font  overlayFont;
    bool      overlayFontLoaded;

    // Полоска HP босса (рисуется поверх HUD)
    bool  bossAlive      = false;
    float bossMaxHP      = 0.f;
    float bossCurrentHP  = 0.f;
    // Сообщение при убийстве босса
    float bossKillMsgTimer = 0.f;

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
    void drawBossBar();   // полоска HP босса вверху экрана
};
