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
#include <vector>

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
    sf::View camera;

    Player player;
    WaveManager waveManager;
    HUD hud;                       // будет инициализирован в конструкторе
    UpgradeSystem upgradeSystem;   // будет инициализирован в конструкторе

    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<ResourceDrop> drops;

    GameState gameState;
    int score;
    float waveClearTimer;

    sf::RectangleShape mapBackground;
    sf::RectangleShape mapBorder;

    SaveData savedData;

    sf::Font overlayFont;
    bool overlayFontLoaded;

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
};