#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include <vector>
#include <queue>

// ============================================================
//  WaveManager.h — управление волнами + босс каждые 5 волн.
//  Разработчик Б.
// ============================================================

class WaveManager {
public:
    WaveManager();

    void startNextWave();

    // Возвращает список врагов для спавна в этом кадре
    std::vector<Enemy> update(float dt);

    bool isWaveFinished(int aliveEnemies) const;

    bool isSpawning()     const { return !spawnQueue.empty(); }
    int  getCurrentWave() const { return currentWave; }
    bool isBossWave()     const;   // true = текущая волна — волна босса

private:
    int   currentWave;
    float spawnTimer;
    float spawnInterval;

    std::queue<EnemyType> spawnQueue;

    void buildSpawnQueue(int wave);
    static sf::Vector2f randomEdgePos();
};
