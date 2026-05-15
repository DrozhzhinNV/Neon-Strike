#include "Game.h"
#include "Constants.h"
#include <utility>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <sstream>

// ============================================================
//  Game.cpp
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
      gameState(GameState::PLAYING), score(0), waveClearTimer(0.f),
      overlayFontLoaded(false),
      hud(window), upgradeSystem(window)
{
    std::srand((unsigned)std::time(nullptr));
    window.setFramerateLimit(C::FPS_LIMIT);

    if (!overlayFont.loadFromFile("assets/font.ttf"))
        overlayFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
    overlayFontLoaded = true;

    mapBackground.setSize({(float)C::MAP_W,(float)C::MAP_H});
    mapBackground.setFillColor(sf::Color(18,24,18));
    mapBorder.setSize({(float)C::MAP_W,(float)C::MAP_H});
    mapBorder.setFillColor(sf::Color::Transparent);
    mapBorder.setOutlineThickness(6.f);
    mapBorder.setOutlineColor(sf::Color(0,200,100,180));

    loadAllTextures();
    tileMap.generate(12345);

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

// ─── Спавн врага с текстурой ─────────────────────────────────
void Game::spawnEnemyWithTexture(Enemy& e) {
    switch (e.getType()) {
        // loadTexture(путь, ширина_кадра, высота_кадра)
        case EnemyType::BASIC:
            e.loadTexture("assets/enemy_basic.png", 48, 48); break;
        case EnemyType::FAST:
            e.loadTexture("assets/enemy_fast.png",  36, 36); break;
        case EnemyType::TANK:
            e.loadTexture("assets/enemy_tank.png",  72, 72); break;
        case EnemyType::BOSS:
            e.loadTexture("assets/enemy_boss.png",  96, 96);
            // Зафиксировать HP босса для полоски
            bossMaxHP     = C::BOSS_HP;
            bossCurrentHP = C::BOSS_HP;
            bossAlive     = true;
            break;
    }
}

// ─── Сохранение ──────────────────────────────────────────────
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
        if (event.type == sf::Event::Closed ||
           (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape))
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
                bossAlive = false;
                waveManager.startNextWave();
            }
        }
    }
}

// ─── Коллизия с объектами карты ──────────────────────────────
sf::Vector2f Game::resolveMapCollision(sf::Vector2f pos, float radius) {
    sf::Vector2f push(0.f,0.f);
    for (const auto& obj : tileMap.getObjects()) {
        if (!obj.solid) continue;
        sf::FloatRect b = obj.bounds;
        float nearX = std::clamp(pos.x, b.left, b.left+b.width);
        float nearY = std::clamp(pos.y, b.top,  b.top+b.height);
        float dx=pos.x-nearX, dy=pos.y-nearY;
        float d=std::sqrt(dx*dx+dy*dy);
        if (d < radius && d > 0.001f) {
            push.x += (dx/d)*(radius-d);
            push.y += (dy/d)*(radius-d);
        }
    }
    return push;
}

// ─── Обновление ──────────────────────────────────────────────
void Game::update(float dt) {
    if (gameState==GameState::GAME_OVER ||
        gameState==GameState::UPGRADE_MENU) return;

    if (gameState==GameState::WAVE_CLEAR) {
        waveClearTimer -= dt;
        if (bossKillMsgTimer > 0.f) bossKillMsgTimer -= dt;
        if (waveClearTimer <= 0.f) {
            upgradeSystem.show(player);
            gameState = GameState::UPGRADE_MENU;
        }
        return;
    }

    if (bossKillMsgTimer > 0.f) bossKillMsgTimer -= dt;

    player.handleInput(dt, window);
    sf::Vector2f push = resolveMapCollision(player.getPosition(),
                                             player.getRadius());
    if (push.x||push.y) {
        player.setPosition(player.getPosition()+push);
        player.clampToMap();
    }

    auto newBullets = player.tryShoot(window);
    for (auto& b : newBullets) bullets.push_back(b);
    updateBullets(dt);

    auto spawned = waveManager.update(dt);
    // Зарезервировать место — чтобы realloc не инвалидировал указатели на texture
    enemies.reserve(enemies.size() + spawned.size());
    for (auto& e : spawned) {
        // Сначала переместить врага в вектор (texture живёт внутри Enemy)
        enemies.push_back(std::move(e));
        // Потом загрузить текстуру прямо в элемент вектора
        spawnEnemyWithTexture(enemies.back());
    }

    updateEnemies(dt);
    checkCollisions();
    collectDrops();
    removeDeadObjects();
    particles.update(dt);
    for (auto& d : drops) if (!d.collected) d.update(dt);

    // Обновить HP босса для полоски
    bossAlive = false;
    for (auto& e : enemies) {
        if (e.isAlive() && e.isBoss()) {
            bossCurrentHP = e.getHealth();
            bossAlive = true;
        }
    }

    applyCamera();
    if (!player.isAlive()) { gameState=GameState::GAME_OVER; return; }

    int alive=0;
    for (auto& e:enemies) if (e.isAlive()) ++alive;
    if (waveManager.isWaveFinished(alive)) {
        waveClearTimer=2.0f;
        gameState=GameState::WAVE_CLEAR;
    }
}

void Game::updateBullets(float dt) {
    for (auto& b:bullets) b.update(dt);
}
void Game::updateEnemies(float dt) {
    sf::Vector2f pp=player.getPosition();
    for (auto& e:enemies) {
        if (!e.isAlive()) continue;
        e.update(dt,pp);
        sf::Vector2f ep=resolveMapCollision(e.getPosition(),e.getRadius());
        if (ep.x||ep.y) e.pushOut(ep);
    }
}

// ─── Коллизии ────────────────────────────────────────────────
void Game::checkCollisions() {
    for (auto& bullet:bullets) {
        if (!bullet.active||!bullet.fromPlayer) continue;
        for (auto& enemy:enemies) {
            if (!enemy.isAlive()) continue;
            if (dist(bullet.getCenter(),enemy.getPosition()) <
                bullet.getRadius()+enemy.getRadius())
            {
                bool died = enemy.takeDamage(bullet.damage);
                bullet.active = false;
                particles.spawnHit(bullet.getCenter());
                if (died) {
                    particles.spawnExplosion(enemy.getPosition(),
                                             enemy.typeColor(),
                                             enemy.isBoss() ? 40 : 20);
                    drops.emplace_back(enemy.getPosition(), enemy.getReward());
                    score += enemy.getReward()*10;

                    // ── Награда за убийство босса ──
                    if (enemy.isBoss()) {
                        player.heal(C::BOSS_HP_REWARD);
                        bossAlive      = false;
                        bossKillMsgTimer = 4.0f;  // показать сообщение 4 сек
                    }
                }
                break;
            }
        }
    }

    for (auto& enemy:enemies) {
        if (!enemy.isAlive()||enemy.damageCooldown>0.f) continue;
        if (dist(enemy.getPosition(),player.getPosition()) <
            enemy.getRadius()+player.getRadius())
        {
            player.takeDamage(enemy.getDamage());
            enemy.damageCooldown=0.8f;
            particles.spawnHit(player.getPosition(),4);
        }
    }
}

void Game::collectDrops() {
    float pr=C::PLAYER_RADIUS+C::RESOURCE_RADIUS+8.f;
    for (auto& drop:drops) {
        if (drop.collected) continue;
        if (dist(drop.getPosition(),player.getPosition())<pr) {
            player.addResource(drop.amount);
            drop.collected=true;
            particles.spawnPickup(drop.getPosition());
        }
    }
}
void Game::removeDeadObjects() {
    bullets.erase(std::remove_if(bullets.begin(),bullets.end(),
        [](const Bullet& b){return !b.active;}),bullets.end());
    enemies.erase(std::remove_if(enemies.begin(),enemies.end(),
        [](const Enemy& e){return !e.isAlive();}),enemies.end());
    drops.erase(std::remove_if(drops.begin(),drops.end(),
        [](const ResourceDrop& r){return r.collected;}),drops.end());
}
void Game::applyCamera() {
    sf::Vector2f pos=player.getPosition();
    float hw=C::WINDOW_W/2.f, hh=C::WINDOW_H/2.f;
    pos.x=std::clamp(pos.x,hw,(float)C::MAP_W-hw);
    pos.y=std::clamp(pos.y,hh,(float)C::MAP_H-hh);
    camera.setCenter(pos);
    window.setView(camera);
}

// ─── Полоска HP босса ─────────────────────────────────────────
void Game::drawBossBar() {
    if (!bossAlive) return;
    sf::View sv = window.getView();
    window.setView(window.getDefaultView());

    const float barW = 500.f, barH = 22.f;
    float bx = (C::WINDOW_W - barW) / 2.f;
    float by = 18.f;

    // Фон
    sf::RectangleShape bg({barW+4.f, barH+4.f});
    bg.setPosition(bx-2.f, by-2.f);
    bg.setFillColor(sf::Color(20,0,0,210));
    bg.setOutlineThickness(2.f);
    bg.setOutlineColor(sf::Color(180,0,180,200));
    window.draw(bg);

    // Заполнение
    float ratio = bossMaxHP > 0.f ? bossCurrentHP/bossMaxHP : 0.f;
    sf::RectangleShape fill({barW*ratio, barH});
    fill.setPosition(bx, by);
    sf::Uint8 rv = (sf::Uint8)(80 + 175*(1.f-ratio));
    fill.setFillColor(sf::Color(rv, 0, 180, 230));
    window.draw(fill);

    // Текст
    if (overlayFontLoaded) {
        sf::Text txt("BOSS", overlayFont, 16);
        txt.setFillColor(sf::Color(255,180,255,230));
        txt.setPosition(bx+8.f, by+3.f);
        window.draw(txt);
    }

    window.setView(sv);
}

// ─── Оверлей ─────────────────────────────────────────────────
void Game::drawOverlay(const std::string& msg, sf::Color color) {
    if (!overlayFontLoaded) return;
    sf::View sv=window.getView();
    window.setView(window.getDefaultView());
    sf::RectangleShape ov({(float)C::WINDOW_W,(float)C::WINDOW_H});
    ov.setFillColor(sf::Color(0,0,0,160));
    window.draw(ov);
    sf::Text text(msg, overlayFont, 52);
    text.setFillColor(color);
    sf::FloatRect b=text.getLocalBounds();
    text.setPosition((C::WINDOW_W-b.width)/2.f,(C::WINDOW_H-b.height)/2.f-30.f);
    sf::Text sh=text;
    sh.setFillColor(sf::Color(0,0,0,180));
    sh.move(3.f,3.f);
    window.draw(sh);
    window.draw(text);
    window.setView(sv);
}

// ─── Отрисовка ───────────────────────────────────────────────
void Game::render() {
    window.clear(sf::Color(30,35,30));

    tileMap.drawGround(window);
    for (auto& drop:drops)   drop.draw(window);
    tileMap.drawObjects(window);
    for (auto& e:enemies)    e.draw(window);
    particles.draw(window);
    for (auto& b:bullets)    b.draw(window);
    player.draw(window);

    hud.draw(player, waveManager, score);
    upgradeSystem.draw();

    // Полоска HP босса (поверх HUD)
    drawBossBar();

    // Сообщение о победе над боссом
    if (bossKillMsgTimer > 0.f && overlayFontLoaded) {
        sf::View sv=window.getView();
        window.setView(window.getDefaultView());
        sf::Text msg("BOSS DEFEATED!  +" +
                     std::to_string((int)C::BOSS_HP_REWARD) + " HP",
                     overlayFont, 36);
        msg.setFillColor(sf::Color(255, 80, 255, 220));
        sf::FloatRect mb=msg.getLocalBounds();
        msg.setPosition((C::WINDOW_W-mb.width)/2.f, 60.f);
        sf::Text sh=msg; sh.move(2.f,2.f);
        sh.setFillColor(sf::Color(0,0,0,150));
        window.draw(sh); window.draw(msg);
        window.setView(sv);
    }

    if (gameState==GameState::WAVE_CLEAR)
        drawOverlay("Wave "+std::to_string(waveManager.getCurrentWave())+
                    " Complete!", sf::Color(80,255,120));
    if (gameState==GameState::GAME_OVER)
        drawOverlay("GAME OVER\nScore: "+std::to_string(score)+
                    "\nPress ENTER to exit", sf::Color(255,80,80));

    window.display();
}
