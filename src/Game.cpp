#include "Game.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <sstream>

static float dist(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

Game::Game()
    : window(sf::VideoMode(C::WINDOW_W, C::WINDOW_H), "WaveShooter",
             sf::Style::Titlebar | sf::Style::Close),
      camera(sf::FloatRect(0.f, 0.f, (float)C::WINDOW_W, (float)C::WINDOW_H)),
      gameState(GameState::PLAYING),
      score(0),
      waveClearTimer(0.f),
      overlayFontLoaded(false),
      hud(window),               // передаём окно в HUD
      upgradeSystem(window)      // и в UpgradeSystem
{
    std::srand((unsigned)std::time(nullptr));
    window.setFramerateLimit(C::FPS_LIMIT);

    if (overlayFont.loadFromFile("assets/font.ttf"))
        overlayFontLoaded = true;
    else if (overlayFont.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        overlayFontLoaded = true;

    mapBackground.setSize({C::MAP_W, C::MAP_H});
    mapBackground.setPosition(0.f, 0.f);
    mapBackground.setFillColor(sf::Color(25, 32, 25));

    mapBorder.setSize({C::MAP_W, C::MAP_H});
    mapBorder.setPosition(0.f, 0.f);
    mapBorder.setFillColor(sf::Color::Transparent);
    mapBorder.setOutlineThickness(6.f);
    mapBorder.setOutlineColor(sf::Color(80, 180, 80));

    if (SaveSystem::load(savedData)) {
        applySaveData();
    }

    waveManager.startNextWave();
}

void Game::applySaveData() {
    score = savedData.score;

    if (savedData.hasShotgun)    player.addWeapon(Weapon::makeShotgun());
    if (savedData.hasMachineGun) player.addWeapon(Weapon::makeMachineGun());

    player.speedMult      = 1.f + savedData.speedLevel  * 0.20f;
    player.damageMult     = 1.f + savedData.damageLevel * 0.25f;
    player.maxHealthBonus =       savedData.healthLevel * 50.f;

    player.addResource(savedData.resources);

    float targetHP = savedData.playerHP;
    player.heal(targetHP - player.getHealth());

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

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            saveGame();
            window.close();
        }

        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape)
        {
            saveGame();
            window.close();
        }

        if (gameState == GameState::GAME_OVER &&
            event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Enter)
        {
            window.close();
        }

        if (gameState == GameState::UPGRADE_MENU &&
            event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            // handleClick теперь сам получает мышь через окно
            if (upgradeSystem.handleClick(player)) {
                gameState = GameState::PLAYING;
                enemies.clear();
                bullets.clear();
                waveManager.startNextWave();
            }
        }
    }
}

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

    auto spawned = waveManager.update(dt);
    for (auto& e : spawned) enemies.push_back(e);

    updateEnemies(dt);
    checkCollisions();
    collectDrops();
    removeDeadObjects();

    applyCamera();

    if (!player.isAlive()) {
        gameState = GameState::GAME_OVER;
        return;
    }

    int aliveCount = 0;
    for (auto& e : enemies) if (e.isAlive()) ++aliveCount;

    if (waveManager.isWaveFinished(aliveCount)) {
        waveClearTimer = 2.0f;
        gameState = GameState::WAVE_CLEAR;
    }
}

void Game::updateBullets(float dt) {
    for (auto& b : bullets) b.update(dt);
}

void Game::updateEnemies(float dt) {
    sf::Vector2f playerPos = player.getPosition();
    for (auto& e : enemies) {
        if (e.isAlive()) e.update(dt, playerPos);
    }
}

void Game::checkCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet.active || !bullet.fromPlayer) continue;

        for (auto& enemy : enemies) {
            if (!enemy.isAlive()) continue;

            float d = dist(bullet.getCenter(), enemy.getPosition());
            if (d < bullet.getRadius() + enemy.getRadius()) {
                bool died = enemy.takeDamage(bullet.damage);
                bullet.active = false;

                if (died) {
                    drops.emplace_back(enemy.getPosition(), enemy.getReward());
                    score += enemy.getReward() * 10;
                }
                break;
            }
        }
    }

    for (auto& enemy : enemies) {
        if (!enemy.isAlive()) continue;
        if (enemy.damageCooldown > 0.f) continue;

        float d = dist(enemy.getPosition(), player.getPosition());
        if (d < enemy.getRadius() + player.getRadius()) {
            player.takeDamage(enemy.getDamage());
            enemy.damageCooldown = 0.8f;
        }
    }
}

void Game::collectDrops() {
    const float pickupRadius = C::PLAYER_RADIUS + C::RESOURCE_RADIUS + 8.f;

    for (auto& drop : drops) {
        if (drop.collected) continue;
        float d = dist(drop.getPosition(), player.getPosition());
        if (d < pickupRadius) {
            player.addResource(drop.amount);
            drop.collected = true;
        }
    }
}

void Game::removeDeadObjects() {
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [](const Bullet& b){ return !b.active; }),
        bullets.end());

    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
                       [](const Enemy& e){ return !e.isAlive(); }),
        enemies.end());

    drops.erase(
        std::remove_if(drops.begin(), drops.end(),
                       [](const ResourceDrop& r){ return r.collected; }),
        drops.end());
}

void Game::applyCamera() {
    sf::Vector2f pos = player.getPosition();

    float halfW = C::WINDOW_W / 2.f;
    float halfH = C::WINDOW_H / 2.f;

    pos.x = std::clamp(pos.x, halfW, C::MAP_W - halfW);
    pos.y = std::clamp(pos.y, halfH, C::MAP_H - halfH);

    camera.setCenter(pos);
    window.setView(camera);
}

void Game::drawBackground() {
    window.draw(mapBackground);

    const float step = 100.f;
    sf::RectangleShape line;
    line.setFillColor(sf::Color(40, 55, 40));

    line.setSize({C::MAP_W, 1.f});
    for (float y = step; y < C::MAP_H; y += step) {
        line.setPosition(0.f, y);
        window.draw(line);
    }
    line.setSize({1.f, C::MAP_H});
    for (float x = step; x < C::MAP_W; x += step) {
        line.setPosition(x, 0.f);
        window.draw(line);
    }

    window.draw(mapBorder);
}

void Game::drawOverlay(const std::string& msg, sf::Color color){
    if (!overlayFontLoaded) return;

    sf::View savedView = window.getView();
    window.setView(window.getDefaultView());

    sf::RectangleShape overlay({(float)C::WINDOW_W, (float)C::WINDOW_H});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    sf::Text text(msg, overlayFont, 52);
    text.setFillColor(color);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition(
        (C::WINDOW_W - bounds.width)  / 2.f,
        (C::WINDOW_H - bounds.height) / 2.f - 30.f
    );
    window.draw(text);

    window.setView(savedView);
}

void Game::render() {
    window.clear(sf::Color(10, 10, 10));

    drawBackground();

    for (auto& drop : drops)   drop.draw(window);
    for (auto& enemy : enemies) enemy.draw(window);
    for (auto& bullet : bullets) bullet.draw(window);
    player.draw(window);

    // HUD и UpgradeSystem рисуются без передачи окна
    hud.draw(player, waveManager, score);
    upgradeSystem.draw();

    if (gameState == GameState::WAVE_CLEAR) {
        drawOverlay("Wave " + std::to_string(waveManager.getCurrentWave())
                    + " Complete!", sf::Color(100, 255, 100));
    }

    if (gameState == GameState::GAME_OVER) {
        drawOverlay("GAME OVER\n\nScore: " + std::to_string(score)
                    + "\n\nPress ENTER to exit",
                    sf::Color(255, 80, 80));
    }

    window.display();
}