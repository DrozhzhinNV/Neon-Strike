#include "Game.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <sstream>

// ============================================================
//  Game.cpp — главный цикл.
//  Разработчик А.
// ============================================================

static float dist(sf::Vector2f a, sf::Vector2f b) {
    float dx=a.x-b.x, dy=a.y-b.y;
    return std::sqrt(dx*dx+dy*dy);
}

// ─── Конструктор ────────────────────────────────────────────
Game::Game()
    : window(sf::VideoMode(C::WINDOW_W, C::WINDOW_H), "Neon Strike",
             sf::Style::Titlebar | sf::Style::Close),
      camera(sf::FloatRect(0.f,0.f,(float)C::WINDOW_W,(float)C::WINDOW_H)),
      gameState(GameState::PLAYING),
      score(0), waveClearTimer(0.f), overlayFontLoaded(false),
      hud(window), upgradeSystem(window)
{
    std::srand((unsigned)std::time(nullptr));
    window.setFramerateLimit(C::FPS_LIMIT);

    if (!overlayFont.loadFromFile("assets/font.ttf"))
        overlayFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
    overlayFontLoaded = true;

    loadAllTextures();

    // Генерация карты (seed фиксирован — карта одинаковая каждый раз)
    tileMap.generate(12345);

    SaveData savedData;
    if (SaveSystem::load(savedData)) applySaveData(savedData);

    waveManager.startNextWave();
}

// ─── Загрузка текстур ────────────────────────────────────────
void Game::loadAllTextures() {
    player.loadTexture("assets/player.png");
    Bullet::loadPlayerTexture("assets/bullet_player.png");
    ResourceDrop::loadTexture("assets/resource.png");
    particles.loadTexture("assets/particle.png");
    tileMap.loadTextures();
}

void Game::spawnEnemyWithTexture(Enemy& e) {
    switch (e.getType()) {
        case EnemyType::BASIC: e.loadTexture("assets/enemy_basic.png"); break;
        case EnemyType::FAST:  e.loadTexture("assets/enemy_fast.png");  break;
        case EnemyType::TANK:  e.loadTexture("assets/enemy_tank.png");  break;
    }
}

// ─── Сохранение / загрузка ───────────────────────────────────
void Game::applySaveData(const SaveData& d) {
    score = d.score;
    if (d.hasShotgun)    player.addWeapon(Weapon::makeShotgun());
    if (d.hasMachineGun) player.addWeapon(Weapon::makeMachineGun());
    player.speedMult      = 1.f + d.speedLevel  * 0.20f;
    player.damageMult     = 1.f + d.damageLevel * 0.25f;
    player.maxHealthBonus =       d.healthLevel * 50.f;
    player.addResource(d.resources);
    player.heal(d.playerHP - player.getHealth());
    for (int i = 0; i < d.wave - 1; ++i) waveManager.startNextWave();
}

void Game::saveGame() {
    SaveData d;
    d.wave          = waveManager.getCurrentWave();
    d.score         = score;
    d.resources     = player.getResources();
    d.playerHP      = player.getHealth();
    d.hasShotgun    = (player.getWeaponCount() >= 2);
    d.hasMachineGun = (player.getWeaponCount() >= 3);
    d.speedLevel  = (int)((player.speedMult  - 1.f) / 0.20f);
    d.damageLevel = (int)((player.damageMult - 1.f) / 0.25f);
    d.healthLevel = (int)(player.maxHealthBonus / 50.f);
    SaveSystem::save(d);
}

// ─── Главный цикл ────────────────────────────────────────────
void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;
        processEvents();
        update(dt);
        render();
    }
}

// ─── События ─────────────────────────────────────────────────
void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            { saveGame(); window.close(); }

        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape)
            { saveGame(); window.close(); }

        if (gameState == GameState::GAME_OVER &&
            event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Enter)
            { window.close(); }

        if (gameState == GameState::UPGRADE_MENU &&
            event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            if (upgradeSystem.handleClick(player)) {
                gameState = GameState::PLAYING;
                enemies.clear(); bullets.clear();
                waveManager.startNextWave();
            }
        }
    }
}

// ─── Коллизия круга с объектами карты ────────────────────────
sf::Vector2f Game::resolveMapCollision(sf::Vector2f pos, float radius) {
    sf::Vector2f push(0.f, 0.f);
    for (const auto& obj : tileMap.getObjects()) {
        if (!obj.solid) continue;
        sf::FloatRect b = obj.bounds;
        // Найти ближайшую точку прямоугольника к центру круга
        float nearX = std::clamp(pos.x, b.left, b.left + b.width);
        float nearY = std::clamp(pos.y, b.top,  b.top  + b.height);
        float dx = pos.x - nearX;
        float dy = pos.y - nearY;
        float d  = std::sqrt(dx*dx + dy*dy);
        if (d < radius && d > 0.001f) {
            // Вытолкнуть из объекта
            float overlap = radius - d;
            push.x += (dx / d) * overlap;
            push.y += (dy / d) * overlap;
        }
    }
    return push;
}

// ─── Обновление ──────────────────────────────────────────────
void Game::update(float dt) {
    if (gameState == GameState::GAME_OVER   ||
        gameState == GameState::UPGRADE_MENU) return;

    if (gameState == GameState::WAVE_CLEAR) {
        waveClearTimer -= dt;
        if (waveClearTimer <= 0.f) {
            upgradeSystem.show(player);
            gameState = GameState::UPGRADE_MENU;
        }
        return;
    }

    // Движение игрока
    player.handleInput(dt, window);

    // Выолкнуть игрока из объектов карты
    sf::Vector2f push = resolveMapCollision(player.getPosition(),
                                             player.getRadius());
    if (push.x != 0.f || push.y != 0.f) {
        sf::Vector2f np = player.getPosition() + push;
        // clamp к карте уже внутри clampToMap
        player.setPosition(np); // нужен сеттер — см. Player.h
        player.clampToMap();
    }

    // Стрельба
    auto newBullets = player.tryShoot(window);
    for (auto& b : newBullets) bullets.push_back(b);

    updateBullets(dt);

    // Спавн врагов
    auto spawned = waveManager.update(dt);
    for (auto& e : spawned) {
        spawnEnemyWithTexture(e);
        enemies.push_back(e);
    }

    updateEnemies(dt);
    checkCollisions();
    collectDrops();
    removeDeadObjects();
    particles.update(dt);
    for (auto& d : drops) if (!d.collected) d.update(dt);

    applyCamera();

    if (!player.isAlive()) { gameState = GameState::GAME_OVER; return; }

    int alive = 0;
    for (auto& e : enemies) if (e.isAlive()) ++alive;
    if (waveManager.isWaveFinished(alive)) {
        waveClearTimer = 2.0f;
        gameState = GameState::WAVE_CLEAR;
    }
}

void Game::updateBullets(float dt) {
    for (auto& b : bullets) b.update(dt);
}

void Game::updateEnemies(float dt) {
    sf::Vector2f pp = player.getPosition();
    for (auto& e : enemies) {
        if (!e.isAlive()) continue;
        e.update(dt, pp);
        // Выталкивать врагов из объектов карты
        sf::Vector2f ep = resolveMapCollision(e.getPosition(), e.getRadius());
        // Враги не «застревают» намертво — просто смещаем
        // (полноценный pathfinding для 2 курса не нужен)
        if (ep.x != 0.f || ep.y != 0.f)
            e.pushOut(ep); // нужен метод в Enemy — см. Enemy.h
    }
}

// ─── Коллизии ────────────────────────────────────────────────
void Game::checkCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet.active || !bullet.fromPlayer) continue;
        for (auto& enemy : enemies) {
            if (!enemy.isAlive()) continue;
            if (dist(bullet.getCenter(), enemy.getPosition()) <
                bullet.getRadius() + enemy.getRadius())
            {
                bool died = enemy.takeDamage(bullet.damage);
                bullet.active = false;
                particles.spawnHit(bullet.getCenter());
                if (died) {
                    particles.spawnExplosion(enemy.getPosition(),
                                             enemy.typeColor(), 22);
                    drops.emplace_back(enemy.getPosition(), enemy.getReward());
                    score += enemy.getReward() * 10;
                }
                break;
            }
        }
    }

    for (auto& enemy : enemies) {
        if (!enemy.isAlive() || enemy.damageCooldown > 0.f) continue;
        if (dist(enemy.getPosition(), player.getPosition()) <
            enemy.getRadius() + player.getRadius())
        {
            player.takeDamage(enemy.getDamage());
            enemy.damageCooldown = 0.8f;
            particles.spawnHit(player.getPosition(), 4);
        }
    }
}

void Game::collectDrops() {
    float pr = C::PLAYER_RADIUS + C::RESOURCE_RADIUS + 8.f;
    for (auto& drop : drops) {
        if (drop.collected) continue;
        if (dist(drop.getPosition(), player.getPosition()) < pr) {
            player.addResource(drop.amount);
            drop.collected = true;
            particles.spawnPickup(drop.getPosition());
        }
    }
}

void Game::removeDeadObjects() {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b){ return !b.active; }), bullets.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e){ return !e.isAlive(); }), enemies.end());
    drops.erase(std::remove_if(drops.begin(), drops.end(),
        [](const ResourceDrop& r){ return r.collected; }), drops.end());
}

void Game::applyCamera() {
    sf::Vector2f pos = player.getPosition();
    float hw = C::WINDOW_W/2.f, hh = C::WINDOW_H/2.f;
    pos.x = std::clamp(pos.x, hw, C::MAP_W - hw);
    pos.y = std::clamp(pos.y, hh, C::MAP_H - hh);
    camera.setCenter(pos);
    window.setView(camera);
}

// ─── Оверлей ─────────────────────────────────────────────────
void Game::drawOverlay(const std::string& msg, sf::Color color) {
    sf::View sv = window.getView();
    window.setView(window.getDefaultView());
    sf::RectangleShape ov({(float)C::WINDOW_W,(float)C::WINDOW_H});
    ov.setFillColor(sf::Color(0,0,0,160));
    window.draw(ov);
    if (overlayFontLoaded) {
        sf::Text text(msg, overlayFont, 52);
        text.setFillColor(color);
        sf::FloatRect b = text.getLocalBounds();
        text.setPosition((C::WINDOW_W-b.width)/2.f,
                         (C::WINDOW_H-b.height)/2.f-30.f);
        sf::Text sh = text;
        sh.setFillColor(sf::Color(0,0,0,180));
        sh.move(3.f, 3.f);
        window.draw(sh);
        window.draw(text);
    }
    window.setView(sv);
}

// ─── Отрисовка ───────────────────────────────────────────────
void Game::render() {
    window.clear(sf::Color(30, 35, 30));

    // 1. Земля (тайлы)
    tileMap.drawGround(window);

    // 2. Монетки (на земле, под объектами)
    for (auto& drop : drops) drop.draw(window);

    // 3. Статические объекты карты (деревья, здания, машины)
    //    Рисуются между землёй и персонажами — правильное перекрытие
    tileMap.drawObjects(window);

    // 4. Враги
    for (auto& e : enemies) e.draw(window);

    // 5. Частицы
    particles.draw(window);

    // 6. Пули
    for (auto& b : bullets) b.draw(window);

    // 7. Игрок поверх всего
    player.draw(window);

    // 8. HUD + апгрейды (экранное пространство)
    hud.draw(player, waveManager, score);
    upgradeSystem.draw();

    if (gameState == GameState::WAVE_CLEAR)
        drawOverlay("Wave " +
            std::to_string(waveManager.getCurrentWave()) + " Complete!",
            sf::Color(80, 255, 120));

    if (gameState == GameState::GAME_OVER)
        drawOverlay("GAME OVER\n\nScore: " + std::to_string(score) +
                    "\n\nPress ENTER to exit", sf::Color(255, 80, 80));

    window.display();
}
