#include "Game.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <sstream>

// ============================================================
//  Game.cpp — главный цикл, загрузка текстур, графика.
//  Разработчик А.
// ============================================================

static float dist(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return std::sqrt(dx*dx + dy*dy);
}

// ─── Конструктор ────────────────────────────────────────────
Game::Game()
    : window(sf::VideoMode(C::WINDOW_W, C::WINDOW_H), "Neon Strike",
             sf::Style::Titlebar | sf::Style::Close),
      camera(sf::FloatRect(0.f, 0.f, (float)C::WINDOW_W, (float)C::WINDOW_H)),
      gameState(GameState::PLAYING),
      score(0), waveClearTimer(0.f), overlayFontLoaded(false),
      hud(window), upgradeSystem(window)
{
    std::srand((unsigned)std::time(nullptr));
    window.setFramerateLimit(C::FPS_LIMIT);

    if (overlayFont.loadFromFile("assets/font.ttf"))
        overlayFontLoaded = true;
    else if (overlayFont.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        overlayFontLoaded = true;

    // Фон (запасной, если текстура не загружена)
    mapBackground.setSize({C::MAP_W, C::MAP_H});
    mapBackground.setPosition(0.f, 0.f);
    mapBackground.setFillColor(sf::Color(18, 24, 18));

    mapBorder.setSize({C::MAP_W, C::MAP_H});
    mapBorder.setPosition(0.f, 0.f);
    mapBorder.setFillColor(sf::Color::Transparent);
    mapBorder.setOutlineThickness(6.f);
    mapBorder.setOutlineColor(sf::Color(0, 200, 100, 180));

    // Загрузить все текстуры
    loadAllTextures();

    if (SaveSystem::load(savedData)) applySaveData();
    waveManager.startNextWave();
}

// ─── Загрузка текстур ────────────────────────────────────────
void Game::loadAllTextures() {
    player.loadTexture("assets/player.png");
    Bullet::loadPlayerTexture("assets/bullet_player.png");
    ResourceDrop::loadTexture("assets/resource.png");
    particles.loadTexture("assets/particle.png");

    // Тайловый фон
    if (bgTexture.loadFromFile("assets/bg_tile.png")) {
        bgTexture.setRepeated(true);   // повторяем тайл по всей карте
        bgTexture.setSmooth(false);
        bgSprite.setTexture(bgTexture);
        bgSprite.setTextureRect(sf::IntRect(0, 0, (int)C::MAP_W, (int)C::MAP_H));
        bgSprite.setPosition(0.f, 0.f);
        bgTexLoaded = true;
    }
}

// ─── Создать врага и сразу загрузить ему текстуру ────────────
void Game::spawnEnemyWithTexture(Enemy& e) {
    switch (e.getType()) {
        case EnemyType::BASIC: e.loadTexture("assets/enemy_basic.png"); break;
        case EnemyType::FAST:  e.loadTexture("assets/enemy_fast.png");  break;
        case EnemyType::TANK:  e.loadTexture("assets/enemy_tank.png");  break;
    }
}

// ─── Применить сохранение ────────────────────────────────────
void Game::applySaveData() {
    score = savedData.score;
    if (savedData.hasShotgun)    player.addWeapon(Weapon::makeShotgun());
    if (savedData.hasMachineGun) player.addWeapon(Weapon::makeMachineGun());
    player.speedMult      = 1.f + savedData.speedLevel  * 0.20f;
    player.damageMult     = 1.f + savedData.damageLevel * 0.25f;
    player.maxHealthBonus =       savedData.healthLevel * 50.f;
    player.addResource(savedData.resources);
    player.heal(savedData.playerHP - player.getHealth());
    for (int i = 0; i < savedData.wave - 1; ++i)
        waveManager.startNextWave();
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
        if (event.type == sf::Event::Closed) { saveGame(); window.close(); }

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
                enemies.clear();
                bullets.clear();
                waveManager.startNextWave();
            }
        }
    }
}

// ─── Обновление ──────────────────────────────────────────────
void Game::update(float dt) {
    if (gameState == GameState::GAME_OVER ||
        gameState == GameState::UPGRADE_MENU) return;

    if (gameState == GameState::WAVE_CLEAR) {
        waveClearTimer -= dt;
        if (waveClearTimer <= 0.f) {
            upgradeSystem.show(player);
            gameState = GameState::UPGRADE_MENU;
        }
        return;
    }

    player.handleInput(dt, window);

    auto newBullets = player.tryShoot(window);
    for (auto& b : newBullets) bullets.push_back(b);

    updateBullets(dt);

    // Спавн новых врагов + загрузка текстур
    auto spawned = waveManager.update(dt);
    for (auto& e : spawned) {
        spawnEnemyWithTexture(e);
        enemies.push_back(e);
    }

    updateEnemies(dt);
    checkCollisions();
    collectDrops();
    removeDeadObjects();

    // Обновить частицы
    particles.update(dt);
    // Обновить монетки (анимация)
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
    for (auto& e : enemies) if (e.isAlive()) e.update(dt, pp);
}

// ─── Коллизии ────────────────────────────────────────────────
void Game::checkCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet.active || !bullet.fromPlayer) continue;
        for (auto& enemy : enemies) {
            if (!enemy.isAlive()) continue;
            float d = dist(bullet.getCenter(), enemy.getPosition());
            if (d < bullet.getRadius() + enemy.getRadius()) {
                bool died = enemy.takeDamage(bullet.damage);
                bullet.active = false;

                // Частицы попадания
                particles.spawnHit(bullet.getCenter());

                if (died) {
                    // Большой взрыв при смерти
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
        float d = dist(enemy.getPosition(), player.getPosition());
        if (d < enemy.getRadius() + player.getRadius()) {
            player.takeDamage(enemy.getDamage());
            enemy.damageCooldown = 0.8f;
            // Маленький взрыв при ударе по игроку
            particles.spawnHit(player.getPosition(), 4);
        }
    }
}

void Game::collectDrops() {
    const float pr = C::PLAYER_RADIUS + C::RESOURCE_RADIUS + 8.f;
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
    float hw = C::WINDOW_W / 2.f, hh = C::WINDOW_H / 2.f;
    pos.x = std::clamp(pos.x, hw, C::MAP_W - hw);
    pos.y = std::clamp(pos.y, hh, C::MAP_H - hh);
    camera.setCenter(pos);
    window.setView(camera);
}

// ─── Фон ─────────────────────────────────────────────────────
void Game::drawBackground() {
    if (bgTexLoaded) {
        window.draw(bgSprite);
    } else {
        window.draw(mapBackground);
        const float step = 100.f;
        sf::RectangleShape line;
        line.setFillColor(sf::Color(35, 52, 35));
        line.setSize({C::MAP_W, 1.f});
        for (float y = step; y < C::MAP_H; y += step) {
            line.setPosition(0.f, y); window.draw(line);
        }
        line.setSize({1.f, C::MAP_H});
        for (float x = step; x < C::MAP_W; x += step) {
            line.setPosition(x, 0.f); window.draw(line);
        }
    }
    window.draw(mapBorder);
}

// ─── Оверлей ─────────────────────────────────────────────────
void Game::drawOverlay(const std::string& msg, sf::Color color) {
    if (!overlayFontLoaded) return;
    sf::View sv = window.getView();
    window.setView(window.getDefaultView());

    sf::RectangleShape ov({(float)C::WINDOW_W, (float)C::WINDOW_H});
    ov.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(ov);

    sf::Text text(msg, overlayFont, 52);
    text.setFillColor(color);
    sf::FloatRect b = text.getLocalBounds();
    text.setPosition((C::WINDOW_W - b.width) / 2.f,
                     (C::WINDOW_H - b.height) / 2.f - 30.f);
    // Тень
    sf::Text shadow = text;
    shadow.setFillColor(sf::Color(0,0,0,180));
    shadow.move(3.f, 3.f);
    window.draw(shadow);
    window.draw(text);

    window.setView(sv);
}

// ─── Отрисовка ───────────────────────────────────────────────
void Game::render() {
    window.clear(sf::Color(8, 10, 8));

    drawBackground();

    // Монетки
    for (auto& drop : drops) drop.draw(window);
    // Враги
    for (auto& enemy : enemies) enemy.draw(window);
    // Пули
    for (auto& bullet : bullets) bullet.draw(window);
    // Частицы (поверх пуль, под игроком)
    particles.draw(window);
    // Игрок
    player.draw(window);

    // HUD и апгрейды — экранное пространство
    hud.draw(player, waveManager, score);
    upgradeSystem.draw();

    if (gameState == GameState::WAVE_CLEAR)
        drawOverlay("Wave " + std::to_string(waveManager.getCurrentWave())
                    + " Complete!", sf::Color(80, 255, 120));

    if (gameState == GameState::GAME_OVER)
        drawOverlay("GAME OVER\n\nScore: " + std::to_string(score)
                    + "\n\nPress ENTER to exit", sf::Color(255, 80, 80));

    window.display();
}
