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
#include <vector>

// ============================================================
//  Game.h — центральный класс.
//  Разработчик А.
// ============================================================

enum class GameState {
    PLAYING,
    WAVE_CLEAR,
    UPGRADE_MENU,
    GAME_OVER,
    WIN
};

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::View         camera;

    Player           player;
    WaveManager      waveManager;
    HUD              hud;
    UpgradeSystem    upgradeSystem;
    ParticleSystem   particles;    // система частиц

    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<ResourceDrop> drops;

    GameState gameState;
    int       score;
    float     waveClearTimer;

    // Фон: тайловый спрайт
    sf::Texture          bgTexture;
    bool                 bgTexLoaded = false;
    sf::RectangleShape   mapBackground;
    sf::RectangleShape   mapBorder;
    sf::Sprite           bgSprite;

    SaveData savedData;

    sf::Font overlayFont;
    bool     overlayFontLoaded;

    void processEvents();
    void update(float dt);
    void render();
    void updateBullets(float dt);
    void updateEnemies(float dt);
    void checkCollisions();
    void collectDrops();
    void removeDeadObjects();
    void applyCamera();
    void drawBackground();
    void saveGame();
    void applySaveData();
    void drawOverlay(const std::string& msg, sf::Color color);
    void loadAllTextures();
    void spawnEnemyWithTexture(Enemy& e);
};
