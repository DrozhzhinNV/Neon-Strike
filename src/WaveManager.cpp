#include "WaveManager.h"
#include "Constants.h"
#include <cstdlib>
#include <algorithm>

// ============================================================
//  WaveManager.cpp — спавн волн врагов + босс.
//  Разработчик Б.
// ============================================================

WaveManager::WaveManager()
    : currentWave(0), spawnTimer(0.f), spawnInterval(0.8f)
{}

bool WaveManager::isBossWave() const {
    return (currentWave > 0) &&
           (currentWave % C::BOSS_WAVE_STEP == 0);
}

void WaveManager::startNextWave() {
    ++currentWave;
    spawnInterval = std::max(0.2f, 0.8f - (float)currentWave * 0.04f);
    buildSpawnQueue(currentWave);
    spawnTimer = 0.f;
}

void WaveManager::buildSpawnQueue(int wave) {
    while (!spawnQueue.empty()) spawnQueue.pop();

    // Волна босса: только обычные враги + один BOSS в конце
    if (wave % C::BOSS_WAVE_STEP == 0) {
        int basic = 4 + wave;
        int fast  = wave / 2;
        for (int i = 0; i < basic; ++i) spawnQueue.push(EnemyType::BASIC);
        for (int i = 0; i < fast;  ++i) spawnQueue.push(EnemyType::FAST);
        // Босс спавнится последним — самое напряжённое время
        spawnQueue.push(EnemyType::BOSS);
        return;
    }

    // Обычная волна
    int basic = 3 + wave * 2;
    int fast  = (wave >= 3) ? 1 + (wave - 3) / 2 : 0;
    int tank  = (wave >= 5) ? 1 + (wave - 5) / 3 : 0;
    for (int i = 0; i < basic; ++i) spawnQueue.push(EnemyType::BASIC);
    for (int i = 0; i < fast;  ++i) spawnQueue.push(EnemyType::FAST);
    for (int i = 0; i < tank;  ++i) spawnQueue.push(EnemyType::TANK);
}

std::vector<Enemy> WaveManager::update(float dt) {
    std::vector<Enemy> newEnemies;
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

sf::Vector2f WaveManager::randomEdgePos() {
    int   edge   = std::rand() % 4;
    float margin = 50.f;
    float x = 0.f, y = 0.f;
    switch (edge) {
        case 0: x = (float)(std::rand()%(int)C::MAP_W); y = -margin;           break;
        case 1: x = (float)(std::rand()%(int)C::MAP_W); y = C::MAP_H + margin; break;
        case 2: x = -margin;           y = (float)(std::rand()%(int)C::MAP_H); break;
        default:x = C::MAP_W + margin; y = (float)(std::rand()%(int)C::MAP_H); break;
    }
    return {x, y};
}
