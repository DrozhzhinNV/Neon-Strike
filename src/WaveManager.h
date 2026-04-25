#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include <vector>
#include <queue>

//  WaveManager.h — управление волнами врагов.


class WaveManager {
public:
    WaveManager();

    // Запустить следующую волну
    void startNextWave();


    std::vector<Enemy> update(float dt); //Спавн врагов по таймеру

    bool isWaveFinished(int aliveEnemies) const;

    bool isSpawning()      const { return !spawnQueue.empty(); }
    int  getCurrentWave()  const { return currentWave; }

private:
    int   currentWave;
    float spawnTimer;
    float spawnInterval; 

    std::queue<EnemyType> spawnQueue;

    void buildSpawnQueue(int wave);

  
    static sf::Vector2f randomEdgePos();
};
