#include "WaveManager.h"
#include "Constants.h"
#include <cstdlib>
#include <algorithm>


//  WaveManager.cpp — спавн волн


WaveManager::WaveManager()
    : currentWave(0), spawnTimer(0.f), spawnInterval(0.8f)
{}

void WaveManager::startNextWave() {
    ++currentWave;

    // С каждой волной спавн ускоряется
    spawnInterval = std::max(0.2f, 0.8f - (float)currentWave * 0.04f);
    buildSpawnQueue(currentWave);
    spawnTimer = 0.f;
}

void WaveManager::buildSpawnQueue(int wave) {
    while (!spawnQueue.empty()) spawnQueue.pop();

    // Кол-во каждого типа
    int basic = 3 + wave * 2; //обычные враги
    int fast  = (wave >= 3) ? 1 + (wave - 3) / 2 : 0; // быстрые враги
    int tank  = (wave >= 5) ? 1 + (wave - 5) / 3 : 0; // толстые враги

    for (int i = 0; i < basic; ++i) spawnQueue.push(EnemyType::BASIC);
    for (int i = 0; i < fast;  ++i) spawnQueue.push(EnemyType::FAST);
    for (int i = 0; i < tank;  ++i) spawnQueue.push(EnemyType::TANK);
}

std::vector<Enemy> WaveManager::update(float dt) {
    std::vector<Enemy> newEnemies;
    //спавним, пока таймер меньше интервала
    if (spawnQueue.empty()) return newEnemies;

    spawnTimer += dt;


    while (spawnTimer >= spawnInterval && !spawnQueue.empty()) {
        spawnTimer -= spawnInterval;
        newEnemies.emplace_back(spawnQueue.front(), randomEdgePos());
        spawnQueue.pop();
    }
    return newEnemies;
}


bool WaveManager::isWaveFinished(int aliveEnemies) const {
    return spawnQueue.empty() && aliveEnemies == 0;
}

//Рандомная позиция спавна врага

sf::Vector2f WaveManager::randomEdgePos() {

    int   edge   = std::rand() % 4;
    float margin = 40.f; 
    float x = 0.f, y = 0.f;

    switch (edge) {
        case 0: 
            x = (float)(std::rand() % (int)C::MAP_W);
            y = -margin;
            break;
        case 1: 
            x = (float)(std::rand() % (int)C::MAP_W);
            y = C::MAP_H + margin;
            break;
        case 2: 
            x = -margin;
            y = (float)(std::rand() % (int)C::MAP_H);
            break;
        default: 
            x = C::MAP_W + margin;
            y = (float)(std::rand() % (int)C::MAP_H);
            break;
    }
    return {x, y};
}
