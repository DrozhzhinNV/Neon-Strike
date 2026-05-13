#include "Particle.h"
#include <cmath>
#include <cstdlib>

// ============================================================
//  Particle.cpp — обновление и отрисовка частиц.
//  Разработчик А.
// ============================================================

// Случайный float в диапазоне [lo, hi]
static float rnd(float lo, float hi) {
    return lo + (float)std::rand() / RAND_MAX * (hi - lo);
}

bool ParticleSystem::loadTexture(const std::string& path) {
    textureLoaded = texture.loadFromFile(path);
    if (textureLoaded) texture.setSmooth(true);
    return textureLoaded;
}

void ParticleSystem::emit(sf::Vector2f pos, sf::Vector2f vel,
                           sf::Color color, float lifetime, float scale)
{
    Particle p;
    if (textureLoaded) {
        p.sprite.setTexture(texture);
        auto sz = texture.getSize();
        p.sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
    }
    p.sprite.setPosition(pos);
    p.sprite.setColor(color);
    p.sprite.setScale(scale, scale);
    p.velocity    = vel;
    p.lifetime    = lifetime;
    p.maxLifetime = lifetime;
    p.rotation    = rnd(-180.f, 180.f);  // случайное вращение
    particles.push_back(p);
}

void ParticleSystem::spawnExplosion(sf::Vector2f pos,
                                     sf::Color color, int count)
{
    for (int i = 0; i < count; ++i) {
        float angle = rnd(0.f, 2.f * 3.14159f);
        float speed = rnd(60.f, 220.f);
        sf::Vector2f vel(std::cos(angle) * speed,
                         std::sin(angle) * speed);
        // немного осветлить цвет для разнообразия
        sf::Color c = color;
        c.r = (sf::Uint8)std::min(255, (int)c.r + (int)rnd(-30, 60));
        c.g = (sf::Uint8)std::min(255, (int)c.g + (int)rnd(-30, 40));
        emit(pos, vel, c, rnd(0.25f, 0.65f), rnd(0.5f, 1.3f));
    }
}

void ParticleSystem::spawnHit(sf::Vector2f pos, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = rnd(0.f, 2.f * 3.14159f);
        float speed = rnd(30.f, 100.f);
        sf::Vector2f vel(std::cos(angle) * speed,
                         std::sin(angle) * speed);
        emit(pos, vel, sf::Color(255, 200, 50, 220),
             rnd(0.1f, 0.25f), rnd(0.3f, 0.7f));
    }
}

void ParticleSystem::spawnPickup(sf::Vector2f pos) {
    for (int i = 0; i < 8; ++i) {
        float angle = rnd(0.f, 2.f * 3.14159f);
        float speed = rnd(20.f, 70.f);
        sf::Vector2f vel(std::cos(angle) * speed,
                         std::sin(angle) * speed);
        emit(pos, vel, sf::Color(0, 230, 180, 210),
             rnd(0.2f, 0.45f), rnd(0.4f, 0.8f));
    }
}

void ParticleSystem::update(float dt) {
    for (auto& p : particles) {
        if (!p.isAlive()) continue;

        p.lifetime -= dt;

        // Затухание: прозрачность пропорциональна оставшейся жизни
        float ratio = std::max(0.f, p.lifetime / p.maxLifetime);
        sf::Color c = p.sprite.getColor();
        c.a = (sf::Uint8)(ratio * 255.f);
        p.sprite.setColor(c);

        // Замедление (трение)
        p.velocity *= (1.f - 3.5f * dt);

        // Движение и вращение
        p.sprite.move(p.velocity * dt);
        p.sprite.rotate(p.rotation * dt);
    }

    // Удалить мёртвые частицы (оставим не больше 600)
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle& p){ return !p.isAlive(); }),
        particles.end());
    if (particles.size() > 600)
        particles.erase(particles.begin(),
                        particles.begin() + (particles.size() - 600));
}

void ParticleSystem::draw(sf::RenderWindow& window) const {
    // Режим наложения Additive: частицы светятся поверх всего
    sf::RenderStates states;
    states.blendMode = sf::BlendAdd;

    for (const auto& p : particles)
        if (p.isAlive())
            window.draw(p.sprite, states);
}
