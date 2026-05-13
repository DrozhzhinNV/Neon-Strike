#pragma once
#include <SFML/Graphics.hpp>

// ============================================================
//  Bullet.h — одна пуля со спрайтом.
//  Разработчик А.
// ============================================================

struct Bullet {
    sf::Sprite   sprite;
    sf::CircleShape fallback;   // запасная форма без текстуры
    sf::Vector2f velocity;
    float        damage;
    bool         active;
    bool         fromPlayer;

    // Статическая текстура (одна на все пули игрока)
    static sf::Texture playerTex;
    static bool        playerTexLoaded;
    static bool        loadPlayerTexture(const std::string& path);

    Bullet(sf::Vector2f pos, sf::Vector2f vel, float dmg, bool player);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    sf::Vector2f getCenter() const;
    float        getRadius() const { return 4.f; }
};
