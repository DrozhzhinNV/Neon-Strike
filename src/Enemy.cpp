#include "Enemy.h"
#include "Constants.h"
#include <cmath>

// ============================================================
//  Enemy.cpp — поведение врагов со спрайтами.
//  Разработчик Б.
// ============================================================

Enemy::Enemy(EnemyType t, sf::Vector2f startPos) : Entity(0.f), type(t) {
    switch (t) {
        case EnemyType::BASIC:
            health = maxHealth = C::BASIC_HP;
            speed         = C::BASIC_SPEED;
            contactDamage = C::BASIC_DAMAGE;
            reward        = C::BASIC_REWARD;
            body.setRadius(C::BASIC_RADIUS);
            body.setFillColor(sf::Color(180, 40, 40, 220));
            body.setOutlineColor(sf::Color(255, 100, 100, 255));
            break;
        case EnemyType::FAST:
            health = maxHealth = C::FAST_HP;
            speed         = C::FAST_SPEED;
            contactDamage = C::FAST_DAMAGE;
            reward        = C::FAST_REWARD;
            body.setRadius(C::FAST_RADIUS);
            body.setFillColor(sf::Color(200, 120, 0, 220));
            body.setOutlineColor(sf::Color(255, 185, 40, 255));
            break;
        case EnemyType::TANK:
            health = maxHealth = C::TANK_HP;
            speed         = C::TANK_SPEED;
            contactDamage = C::TANK_DAMAGE;
            reward        = C::TANK_REWARD;
            body.setRadius(C::TANK_RADIUS);
            body.setFillColor(sf::Color(90, 25, 150, 220));
            body.setOutlineColor(sf::Color(170, 70, 255, 255));
            break;
    }
    body.setOutlineThickness(2.5f);
    body.setOrigin(body.getRadius(), body.getRadius());
    body.setPosition(startPos);
}

bool Enemy::loadTexture(const std::string& path) {
    textureLoaded = texture.loadFromFile(path);
    if (textureLoaded) {
        texture.setSmooth(true);
        sprite.setTexture(texture);
        auto sz = texture.getSize();
        sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
        // Масштаб: вписать текстуру в радиус тела
        float desired = body.getRadius() * 2.2f;
        float scale   = desired / sz.x;
        sprite.setScale(scale, scale);
        sprite.setPosition(body.getPosition());
    }
    return textureLoaded;
}

sf::Color Enemy::typeColor() const {
    switch (type) {
        case EnemyType::BASIC: return sf::Color(220, 60, 60);
        case EnemyType::FAST:  return sf::Color(220, 150, 0);
        case EnemyType::TANK:  return sf::Color(130, 40, 200);
    }
    return sf::Color::White;
}

void Enemy::update(float dt, sf::Vector2f playerPos) {
    if (damageCooldown > 0.f) damageCooldown -= dt;

    sf::Vector2f dir = playerPos - body.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.f) dir /= len;

    sf::Vector2f delta = dir * speed * dt;
    body.move(delta);

    // Угол движения → поворот спрайта (враги смотрят на игрока)
    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;
    // Медленное вращение у танка, быстрое у быстрого
    float rotSpeed = (type == EnemyType::TANK) ? 40.f :
                     (type == EnemyType::FAST) ? 200.f : 90.f;
    rotationAngle += rotSpeed * dt;

    if (textureLoaded) {
        sprite.setPosition(body.getPosition());
        sprite.setRotation(angle + rotationAngle);
    }
}

bool Enemy::takeDamage(float amount) {
    applyDamage(amount);
    // Вспышка белого при ударе
    if (textureLoaded) {
        float ratio = health / maxHealth;
        // Краснеем при малом HP
        sf::Uint8 r = 255;
        sf::Uint8 g = (sf::Uint8)(ratio * 200.f);
        sf::Uint8 b = (sf::Uint8)(ratio * 200.f);
        sprite.setColor(sf::Color(r, g, b, 230));
    }
    return health <= 0.f;
}

void Enemy::drawHealthBar(sf::RenderWindow& window) const {
    if (health >= maxHealth) return;
    float r    = body.getRadius();
    float barW = r * 2.2f;
    float barH = 5.f;
    sf::Vector2f pos = body.getPosition();
    float bx = pos.x - barW / 2.f;
    float by = pos.y - r - 12.f;

    sf::RectangleShape bg({barW, barH});
    bg.setPosition(bx, by);
    bg.setFillColor(sf::Color(70, 70, 70, 180));
    window.draw(bg);

    float ratio = health / maxHealth;
    sf::RectangleShape fill({barW * ratio, barH});
    fill.setPosition(bx, by);
    auto g  = (sf::Uint8)(200.f * ratio);
    auto r2 = (sf::Uint8)(50.f + 200.f * (1.f - ratio));
    fill.setFillColor(sf::Color(r2, g, 30));
    window.draw(fill);
}

void Enemy::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        // Ореол свечения (Additive blend, большой полупрозрачный спрайт)
        sf::RenderStates glowState;
        glowState.blendMode = sf::BlendAdd;
        sf::Sprite halo = sprite;
        halo.setScale(sprite.getScale() * 1.5f);
        sf::Color hc = typeColor();
        hc.a = 50;
        halo.setColor(hc);
        window.draw(halo, glowState);
        // Сам спрайт
        window.draw(sprite);
    } else {
        window.draw(body);
    }
    drawHealthBar(window);
}