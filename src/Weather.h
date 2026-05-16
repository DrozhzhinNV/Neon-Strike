#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// ============================================================
//  Weather.h — система погоды: дождь, гроза, молнии.
//
//  Режимы:
//    CLEAR  — сухо (переходной, не используется в игре)
//    RAIN   — лёгкий дождь (обычные волны)
//    STORM  — ливень + молнии (волна босса)
//
//  Дождь рисуется в ЭКРАННЫХ координатах поверх всей сцены
//  через полупрозрачный overlay — не зависит от камеры.
//  Молния — мгновенная яркая вспышка + зигзаг на небе.
//  Разработчик А.
// ============================================================

enum class WeatherMode { CLEAR, RAIN, STORM };

struct RainDrop {
    sf::Vector2f pos;      // экранная позиция
    float        speed;    // px/сек
    float        len;      // длина черты
    float        alpha;    // прозрачность 0..255
};

struct LightningBolt {
    // Набор отрезков зигзага
    std::vector<sf::Vector2f> points;
    float lifetime;    // сколько живёт (сек)
    float maxLifetime;
    bool  active;
};

class Weather {
public:
    Weather();

    // Установить режим с плавным переходом
    void setMode(WeatherMode mode);
    WeatherMode getMode() const { return targetMode; }

    // Обновление каждый кадр (dt — время кадра)
    void update(float dt);

    // Рисовать в ЭКРАННЫХ координатах (вызывать после window.setView(defaultView))
    void draw(sf::RenderWindow& window) const;

    // Загрузить звуки (опционально — если есть файлы)
    // void loadSounds(...);  ← SFML Audio опционален, пока без звука

private:
    WeatherMode currentMode = WeatherMode::CLEAR;
    WeatherMode targetMode  = WeatherMode::CLEAR;

    // ── Дождь ────────────────────────────────────────────────
    std::vector<RainDrop> drops;
    float dropSpawnTimer = 0.f;

    // Текущее количество капель (плавно меняется при смене режима)
    float currentDropCount = 0.f;   // 0..MAX_DROPS
    float targetDropCount  = 0.f;

    static constexpr int   MAX_DROPS      = 600;
    static constexpr float RAIN_COUNT     = 120.f;  // лёгкий дождь
    static constexpr float STORM_COUNT    = 550.f;  // ливень

    // Угол наклона дождя (немного вправо)
    float windAngle = 15.f;   // градусы от вертикали
    float targetWindAngle = 15.f;

    // ── Затемнение неба ───────────────────────────────────────
    float skyDarkness     = 0.f;   // 0..1
    float targetDarkness  = 0.f;

    // ── Молнии ───────────────────────────────────────────────
    std::vector<LightningBolt> bolts;
    float lightningTimer  = 0.f;   // таймер до следующей молнии
    float lightningDelay  = 0.f;   // случайный интервал
    float flashAlpha      = 0.f;   // яркость вспышки (затухает)

    // ── Вспомогательные методы ───────────────────────────────
    void spawnRainDrop(float screenW, float screenH);
    void spawnLightning(float screenW, float screenH);
    void updateDrops(float dt, float screenW, float screenH);
    void updateLightning(float dt, float screenW, float screenH);
    static std::vector<sf::Vector2f> buildBolt(
        sf::Vector2f start, sf::Vector2f end, int depth);
};
