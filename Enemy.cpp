#include "Enemy.h"
#include "Constants.h"
#include <cmath>

//  Enemy.cpp — поведение врагов.


Enemy::Enemy(EnemyType t, sf::Vector2f startPos) : type(t) {
    // Пропишу параметры врагов в зависимости от их типа
    // 3 типа: дефолтный враг, быстрый и толстый
    switch (t) {
        case EnemyType::BASIC:
            health = maxHealth = C::BASIC_HP;
            speed         = C::BASIC_SPEED;
            contactDamage = C::BASIC_DAMAGE;
            reward        = C::BASIC_REWARD;
            body.setRadius(C::BASIC_RADIUS);
            body.setFillColor(sf::Color(200, 50,  50));
            body.setOutlineColor(sf::Color(255, 110, 110));
            break;

        case EnemyType::FAST:
            health = maxHealth = C::FAST_HP;
            speed         = C::FAST_SPEED;
            contactDamage = C::FAST_DAMAGE;
            reward        = C::FAST_REWARD;
            body.setRadius(C::FAST_RADIUS);
            body.setFillColor(sf::Color(220, 140, 0));
            body.setOutlineColor(sf::Color(255, 190, 40));
            break;

        case EnemyType::TANK:
            health = maxHealth = C::TANK_HP;
            speed         = C::TANK_SPEED;
            contactDamage = C::TANK_DAMAGE;
            reward        = C::TANK_REWARD;
            body.setRadius(C::TANK_RADIUS);
            body.setFillColor(sf::Color(100, 30, 160));
            body.setOutlineColor(sf::Color(170, 80, 255));
            break;
    }

    body.setOutlineThickness(2.5f);
    body.setOrigin(body.getRadius(), body.getRadius());
    body.setPosition(startPos);
}

void Enemy::update(float dt, sf::Vector2f playerPos) {

    if (damageCooldown > 0.f) damageCooldown -= dt;


    sf::Vector2f dir = playerPos - body.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.f) dir /= len;

    body.move(dir * speed * dt);
}

bool Enemy::takeDamage(float amount) {
    health -= amount;
    if (health < 0.f) health = 0.f;
    return health <= 0.f; 
}

void Enemy::drawHealthBar(sf::RenderWindow& window) const {

    if (health >= maxHealth) return;

    float r    = body.getRadius();
    float barW = r * 2.2f;
    float barH = 5.f;
    sf::Vector2f pos = body.getPosition();
    float bx = pos.x - barW / 2.f;
    float by = pos.y - r - 10.f;


    sf::RectangleShape bg({barW, barH});
    bg.setPosition(bx, by);
    bg.setFillColor(sf::Color(70, 70, 70));
    window.draw(bg);


    float ratio = health / maxHealth;
    sf::RectangleShape fill({barW * ratio, barH});
    fill.setPosition(bx, by);

    auto g = (sf::Uint8)(200.f * ratio);
    auto r2= (sf::Uint8)(50.f + 200.f * (1.f - ratio));
    fill.setFillColor(sf::Color(r2, g, 30));
    window.draw(fill);
}

void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(body);
    drawHealthBar(window);
}
