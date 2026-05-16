#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

// ============================================================
//  ResourceDrop.h — выпадающий предмет: монетка или патроны.
//  Разработчик Б.
// ============================================================

enum class DropType {
    MONEY,   // зелёная монетка → ресурсы
    AMMO     // жёлтый патрон  → пули
};

struct ResourceDrop {
    sf::Sprite      sprite;
    sf::CircleShape fallback;
    DropType        dropType;
    int             amount;    // монеты или патроны
    bool            collected;
    float           bobTimer = 0.f;

    // Статические текстуры
    static sf::Texture texMoney;
    static sf::Texture texAmmo;
    static bool        texLoaded;
    static bool        loadTextures(const std::string& moneyPath,
                                    const std::string& ammoPath);

    ResourceDrop(sf::Vector2f pos, int amt, DropType type = DropType::MONEY);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    sf::Vector2f getPosition() const;
    float        getRadius()   const { return C::RESOURCE_RADIUS; }
};
