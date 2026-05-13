#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// ============================================================
//  Particle.h — система частиц (взрывы, вспышки, след пуль).
//  Разработчик А.
// ============================================================

struct Particle {
    sf::Sprite  sprite;
    sf::Vector2f velocity;
    float        lifetime;     // сколько жить (сек)
    float        maxLifetime;
    float        rotation;     // скорость вращения (градус/сек)

    bool isAlive() const { return lifetime > 0.f; }
};

class ParticleSystem {
public:
    // Загрузить текстуру частицы один раз
    bool loadTexture(const std::string& path);

    // Взрыв при смерти врага: много частиц во все стороны
    void spawnExplosion(sf::Vector2f pos, sf::Color color, int count = 18);

    // Вспышка при попадании пули
    void spawnHit(sf::Vector2f pos, int count = 6);

    // Подбор монетки
    void spawnPickup(sf::Vector2f pos);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    int count() const { return (int)particles.size(); }

private:
    sf::Texture              texture;
    bool                     textureLoaded = false;
    std::vector<Particle>    particles;

    void emit(sf::Vector2f pos, sf::Vector2f vel,
              sf::Color color, float lifetime, float scale = 1.f);
};
