#include "Weather.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

// ============================================================
//  Weather.cpp — дождь, ливень, молнии.
//  Разработчик А.
// ============================================================

// ── Случайные числа ──────────────────────────────────────────
static float rndF(float lo, float hi) {
    return lo + (float)std::rand() / RAND_MAX * (hi - lo);
}
static int rndI(int lo, int hi) {      // [lo, hi)
    return lo + std::rand() % (hi - lo);
}

// ─────────────────────────────────────────────────────────────
Weather::Weather() {
    // Зарезервировать место, чтобы не было realloc
    drops.reserve(MAX_DROPS + 50);
    bolts.reserve(8);
    lightningDelay = rndF(4.f, 8.f);
}

// ── Установить режим ─────────────────────────────────────────
void Weather::setMode(WeatherMode mode) {
    if (targetMode == mode) return;
    targetMode = mode;

    switch (mode) {
        case WeatherMode::CLEAR:
            targetDropCount  = 0.f;
            targetDarkness   = 0.f;
            targetWindAngle  = 15.f;
            break;
        case WeatherMode::RAIN:
            targetDropCount  = RAIN_COUNT;
            targetDarkness   = 0.18f;   // лёгкое потемнение
            targetWindAngle  = 12.f;
            break;
        case WeatherMode::STORM:
            targetDropCount  = STORM_COUNT;
            targetDarkness   = 0.48f;   // сильное потемнение
            targetWindAngle  = 25.f;    // ветер сильнее
            lightningTimer   = 1.5f;    // первая молния скоро
            lightningDelay   = rndF(2.f, 5.f);
            break;
    }
    currentMode = mode;
}

// ── Спавн одной капли ────────────────────────────────────────
void Weather::spawnRainDrop(float W, float H) {
    if ((int)drops.size() >= MAX_DROPS) return;

    RainDrop d;
    // Капли появляются за верхним краем и немного за левым
    // (с учётом наклона ветра)
    float margin = H * std::tan(targetWindAngle * 3.14159f / 180.f) + 30.f;
    d.pos.x  = rndF(-margin, W + 10.f);
    d.pos.y  = rndF(-40.f, -5.f);
    d.speed  = rndF(520.f, 820.f);
    d.len    = rndF(8.f, 20.f);
    // При ливне капли ярче
    float baseAlpha = (targetMode == WeatherMode::STORM) ? 140.f : 70.f;
    d.alpha  = rndF(baseAlpha * 0.6f, baseAlpha);
    drops.push_back(d);
}

// ── Обновить капли ───────────────────────────────────────────
void Weather::updateDrops(float dt, float W, float H) {
    float angleRad = windAngle * 3.14159f / 180.f;
    float dx = std::sin(angleRad);   // горизонтальный компонент (нормирован)
    float dy = std::cos(angleRad);   // вертикальный компонент

    for (auto& d : drops) {
        d.pos.x += dx * d.speed * dt;
        d.pos.y += dy * d.speed * dt;
    }

    // Удалить капли ушедшие за нижний / правый край
    drops.erase(
        std::remove_if(drops.begin(), drops.end(),
            [&](const RainDrop& d){
                return d.pos.y > H + 20.f || d.pos.x > W + 20.f;
            }),
        drops.end());

    // Спавнить новые капли чтобы поддерживать targetDropCount
    float needed = targetDropCount - (float)drops.size();
    if (needed > 0.f) {
        // За один кадр добавляем пропорционально needed
        int spawnThis = (int)(needed * dt * 8.f) + 1;
        for (int i = 0; i < spawnThis && (int)drops.size() < MAX_DROPS; ++i)
            spawnRainDrop(W, H);
    }
}

// ── Построить зигзаг молнии (рекурсивно) ────────────────────
std::vector<sf::Vector2f> Weather::buildBolt(
    sf::Vector2f start, sf::Vector2f end, int depth)
{
    std::vector<sf::Vector2f> pts;
    if (depth == 0) {
        pts.push_back(start);
        pts.push_back(end);
        return pts;
    }
    // Середина + случайное смещение
    sf::Vector2f mid = (start + end) * 0.5f;
    sf::Vector2f perp = { -(end.y - start.y), end.x - start.x };
    float perpLen = std::sqrt(perp.x*perp.x + perp.y*perp.y);
    if (perpLen > 0.001f) perp /= perpLen;
    float offset = rndF(-60.f, 60.f) * (float)(1 << (depth-1)) / 4.f;
    mid += perp * offset;

    auto left  = buildBolt(start, mid, depth-1);
    auto right = buildBolt(mid,   end, depth-1);

    pts.insert(pts.end(), left.begin(),  left.end());
    pts.insert(pts.end(), right.begin(), right.end());
    return pts;
}

// ── Спавн молнии ─────────────────────────────────────────────
void Weather::spawnLightning(float W, float H) {
    LightningBolt bolt;
    // Начало — случайная точка вверху экрана
    sf::Vector2f start(rndF(W*0.1f, W*0.9f), rndF(-20.f, 40.f));
    // Конец — ближе к земле, небольшое горизонтальное смещение
    sf::Vector2f end(start.x + rndF(-120.f, 120.f),
                     rndF(H*0.35f, H*0.75f));

    bolt.points     = buildBolt(start, end, 4);  // 4 уровня рекурсии
    bolt.lifetime   = rndF(0.08f, 0.20f);
    bolt.maxLifetime= bolt.lifetime;
    bolt.active     = true;
    bolts.push_back(bolt);

    // Вспышка
    flashAlpha = 220.f;
}

// ── Обновить молнии ──────────────────────────────────────────
void Weather::updateLightning(float dt, float W, float H) {
    if (targetMode != WeatherMode::STORM) {
        bolts.clear();
        flashAlpha = std::max(0.f, flashAlpha - dt * 600.f);
        return;
    }

    // Таймер до следующей молнии
    lightningTimer -= dt;
    if (lightningTimer <= 0.f) {
        spawnLightning(W, H);
        lightningDelay  = rndF(2.5f, 6.f);
        lightningTimer  = lightningDelay;

        // Иногда вторая молния быстро следом
        if (std::rand() % 3 == 0) lightningTimer = rndF(0.15f, 0.5f);
    }

    // Обновить существующие болты
    for (auto& b : bolts) {
        b.lifetime -= dt;
        if (b.lifetime <= 0.f) b.active = false;
    }
    bolts.erase(std::remove_if(bolts.begin(), bolts.end(),
        [](const LightningBolt& b){ return !b.active; }), bolts.end());

    // Затухание вспышки
    flashAlpha = std::max(0.f, flashAlpha - dt * 500.f);
}

// ── Главное обновление ───────────────────────────────────────
void Weather::update(float dt) {
    // Плавная интерполяция параметров
    float smooth = 1.f - std::pow(0.01f, dt);  // экспоненциальное сглаживание

    currentDropCount += (targetDropCount - currentDropCount) * smooth * 2.f;
    skyDarkness      += (targetDarkness  - skyDarkness)      * smooth * 1.5f;
    windAngle        += (targetWindAngle - windAngle)         * smooth;

    float W = 1024.f, H = 768.f;   // размер экрана (совпадает с C::WINDOW_W/H)
    updateDrops(dt, W, H);
    updateLightning(dt, W, H);
}

// ── Отрисовка ────────────────────────────────────────────────
void Weather::draw(sf::RenderWindow& window) const {
    if (targetMode == WeatherMode::CLEAR && skyDarkness < 0.01f) return;

    // ── 1. Затемнение неба ────────────────────────────────────
    if (skyDarkness > 0.01f) {
        sf::RectangleShape sky({1024.f, 768.f});
        sky.setFillColor(sf::Color(10, 15, 30,
                         (sf::Uint8)(skyDarkness * 180.f)));
        window.draw(sky);
    }

    // ── 2. Вспышка молнии ─────────────────────────────────────
    if (flashAlpha > 1.f) {
        sf::RectangleShape flash({1024.f, 768.f});
        flash.setFillColor(sf::Color(200, 215, 255,
                           (sf::Uint8)std::min(flashAlpha, 255.f)));
        sf::RenderStates addState;
        addState.blendMode = sf::BlendAdd;
        window.draw(flash, addState);
    }

    // ── 3. Молнии (зигзаги) ───────────────────────────────────
    for (const auto& bolt : bolts) {
        if (!bolt.active || bolt.points.size() < 2) continue;

        float ratio = bolt.lifetime / bolt.maxLifetime;
        // Основной белый зигзаг
        for (size_t i = 0; i + 1 < bolt.points.size(); i += 2) {
            sf::Vertex line[2];
            line[0].position = bolt.points[i];
            line[1].position = bolt.points[i+1];
            // Ядро — яркое белое/голубое
            sf::Color core(220, 235, 255, (sf::Uint8)(ratio * 255.f));
            line[0].color = core;
            line[1].color = core;
            window.draw(line, 2, sf::Lines);
        }
        // Свечение вокруг — широкая полупрозрачная линия
        for (size_t i = 0; i + 1 < bolt.points.size(); i += 2) {
            sf::Vertex glow[2];
            glow[0].position = bolt.points[i];
            glow[1].position = bolt.points[i+1];
            sf::Color gc(100, 140, 255, (sf::Uint8)(ratio * 80.f));
            glow[0].color = gc;
            glow[1].color = gc;
            sf::RenderStates addSt;
            addSt.blendMode = sf::BlendAdd;
            window.draw(glow, 2, sf::Lines, addSt);
        }
    }

    // ── 4. Капли дождя ────────────────────────────────────────
    if (drops.empty()) return;

    float angleRad = windAngle * 3.14159f / 180.f;
    float dx = std::sin(angleRad);
    float dy = std::cos(angleRad);

    // Используем VertexArray для эффективной отрисовки
    sf::VertexArray va(sf::Lines, drops.size() * 2);
    for (size_t i = 0; i < drops.size(); ++i) {
        const auto& d = drops[i];
        sf::Color col(180, 210, 255, (sf::Uint8)d.alpha);
        sf::Color colFade(180, 210, 255, 0);

        va[i*2].position     = d.pos;
        va[i*2].color        = col;
        va[i*2+1].position   = {d.pos.x - dx*d.len, d.pos.y - dy*d.len};
        va[i*2+1].color      = colFade;   // хвост прозрачный — красиво
    }
    sf::RenderStates rainState;
    rainState.blendMode = sf::BlendAdd;
    window.draw(va, rainState);
}
