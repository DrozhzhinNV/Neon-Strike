#pragma once
#include "Constants.h"
#include <SFML/Graphics.hpp>

// ============================================================
//  ResourceDrop.h — монетка со спрайтом.
//  Разработчик Б.
// ============================================================

struct ResourceDrop {
    sf::Sprite   sprite;
    sf::CircleShape fallback;
    int          amount;
    bool         collected;
    float        bobTimer = 0.f;   // анимация "плавания" вверх-вниз

    static sf::Texture tex;
    static bool        texLoaded;
    static bool        loadTexture(const std::string& path);

    ResourceDrop(sf::Vector2f pos, int amt);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    sf::Vector2f getPosition() const;
    float        getRadius()   const { return C::RESOURCE_RADIUS; }
};
