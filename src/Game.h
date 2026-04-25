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


//  Game.h — центральный класс, объединяет все системы.


// Состояния игры
enum class GameState {
    PLAYING,       // идёт волна
    WAVE_CLEAR,    // волна окончена, пауза перед апгрейдом
    UPGRADE_MENU,  // выбор апгрейда
    GAME_OVER,     // игрок погиб
    WIN            // (опционально: победа после N волн)
};

class Game {
public:
    Game();
    void run(); // главный цикл

private:
    sf::RenderWindow window;
    sf::View         camera;   // камера следует за игроком

    // --- Игровые объекты ---
    Player      player;
    WaveManager waveManager;
    HUD         hud;
    UpgradeSystem upgradeSystem;

    std::vector<Enemy>        enemies;
    std::vector<Bullet>       bullets;
    std::vector<ResourceDrop> drops;

    // --- Состояние ---
    GameState gameState;
    int       score;
    float     waveClearTimer; // задержка 2 сек перед меню апгрейдов

    // --- Фон карты ---
    sf::RectangleShape mapBackground;
    sf::RectangleShape mapBorder;    // рамка карты

    // --- Сохранение ---
    SaveData savedData;

    // --- Вспомогательные методы ---
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

    // Сообщение на весь экран (Game Over / Wave Clear)
    void drawOverlay(const std::string& msg, sf::Color color);

    sf::Font overlayFont;
    bool     overlayFontLoaded;
};
