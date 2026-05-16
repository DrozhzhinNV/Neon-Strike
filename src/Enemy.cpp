#include "Enemy.h"
#include "Constants.h"
#include <cmath>

// ============================================================
//  Enemy.cpp
//  Разработчик Б.
// ============================================================

// ─── Конструктор ─────────────────────────────────────────────
Enemy::Enemy(EnemyType t, sf::Vector2f startPos)
    : Entity(0.f), type(t)
{
    switch (t) {
        case EnemyType::BASIC:
            health = maxHealth = C::BASIC_HP;
            speed = C::BASIC_SPEED; contactDamage = C::BASIC_DAMAGE;
            reward = C::BASIC_REWARD;
            body.setRadius(C::BASIC_RADIUS);
            body.setFillColor(sf::Color(180,40,40,220));
            body.setOutlineColor(sf::Color(255,100,100,255));
            animSpeed = 0.13f;
            break;
        case EnemyType::FAST:
            health = maxHealth = C::FAST_HP;
            speed = C::FAST_SPEED; contactDamage = C::FAST_DAMAGE;
            reward = C::FAST_REWARD;
            body.setRadius(C::FAST_RADIUS);
            body.setFillColor(sf::Color(200,120,0,220));
            body.setOutlineColor(sf::Color(255,185,40,255));
            animSpeed = 0.07f;
            break;
        case EnemyType::TANK:
            health = maxHealth = C::TANK_HP;
            speed = C::TANK_SPEED; contactDamage = C::TANK_DAMAGE;
            reward = C::TANK_REWARD;
            body.setRadius(C::TANK_RADIUS);
            body.setFillColor(sf::Color(90,25,150,220));
            body.setOutlineColor(sf::Color(170,70,255,255));
            animSpeed = 0.20f;
            break;
        case EnemyType::BOSS:
            health = maxHealth = C::BOSS_HP;
            speed = C::BOSS_SPEED; contactDamage = C::BOSS_DAMAGE;
            reward = C::BOSS_REWARD;
            // Радиус коллизии МЕНЬШЕ визуального — босс не застревает в деревьях.
            // Спрайт всё равно рисуется крупным через BOSS_SPRITE_SCALE.
            body.setRadius(C::BOSS_RADIUS * 0.55f);
            body.setFillColor(sf::Color(140,0,0,230));
            body.setOutlineColor(sf::Color(255,40,40,255));
            animSpeed = 0.18f;
            break;
    }
    body.setOutlineThickness(3.f);
    body.setOrigin(body.getRadius(), body.getRadius());
    body.setPosition(startPos);
}

// ─── Перепривязать sprite к своей texture ────────────────────
//  Вызывается после каждого копирования / перемещения,
//  чтобы sprite.getTexture() указывал на наш объект texture,
//  а не на texture исходного объекта.
void Enemy::rebindSprite() {
    if (textureLoaded) {
        sprite.setTexture(texture, /*resetRect=*/false);
    }
}

// ─── Copy constructor ────────────────────────────────────────
Enemy::Enemy(const Enemy& o)
    : Entity(o.maxHealth),
      type(o.type), body(o.body), texture(o.texture), sprite(o.sprite),
      textureLoaded(o.textureLoaded),
      frameW(o.frameW), frameH(o.frameH),
      animFrame(o.animFrame), animTimer(o.animTimer), animSpeed(o.animSpeed),
      aimAngle(o.aimAngle), speed(o.speed),
      contactDamage(o.contactDamage), reward(o.reward),
      damageCooldown(o.damageCooldown)
{
    health = o.health;
    rebindSprite(); // ← ключевая строка
}

// ─── Copy assignment ─────────────────────────────────────────
Enemy& Enemy::operator=(const Enemy& o) {
    if (this == &o) return *this;
    // Поля базового класса
    health    = o.health;
    maxHealth = o.maxHealth;
    // Поля этого класса
    type          = o.type;
    body          = o.body;
    texture       = o.texture;
    sprite        = o.sprite;
    textureLoaded = o.textureLoaded;
    frameW        = o.frameW;
    frameH        = o.frameH;
    animFrame     = o.animFrame;
    animTimer     = o.animTimer;
    animSpeed     = o.animSpeed;
    aimAngle      = o.aimAngle;
    speed         = o.speed;
    contactDamage = o.contactDamage;
    reward        = o.reward;
    damageCooldown= o.damageCooldown;
    rebindSprite(); // ← ключевая строка
    return *this;
}

// ─── Move constructor ────────────────────────────────────────
Enemy::Enemy(Enemy&& o) noexcept
    : Entity(o.maxHealth),
      type(o.type), body(std::move(o.body)),
      texture(std::move(o.texture)), sprite(std::move(o.sprite)),
      textureLoaded(o.textureLoaded),
      frameW(o.frameW), frameH(o.frameH),
      animFrame(o.animFrame), animTimer(o.animTimer), animSpeed(o.animSpeed),
      aimAngle(o.aimAngle), speed(o.speed),
      contactDamage(o.contactDamage), reward(o.reward),
      damageCooldown(o.damageCooldown)
{
    health = o.health;
    o.textureLoaded = false;
    rebindSprite(); // ← ключевая строка
}

// ─── Move assignment ─────────────────────────────────────────
Enemy& Enemy::operator=(Enemy&& o) noexcept {
    if (this == &o) return *this;
    health        = o.health;
    maxHealth     = o.maxHealth;
    type          = o.type;
    body          = std::move(o.body);
    texture       = std::move(o.texture);
    sprite        = std::move(o.sprite);
    textureLoaded = o.textureLoaded;
    frameW        = o.frameW;
    frameH        = o.frameH;
    animFrame     = o.animFrame;
    animTimer     = o.animTimer;
    animSpeed     = o.animSpeed;
    aimAngle      = o.aimAngle;
    speed         = o.speed;
    contactDamage = o.contactDamage;
    reward        = o.reward;
    damageCooldown= o.damageCooldown;
    o.textureLoaded = false;
    rebindSprite(); // ← ключевая строка
    return *this;
}

// ─── Загрузка спрайт-листа ───────────────────────────────────
bool Enemy::loadTexture(const std::string& path, int fw, int fh) {
    textureLoaded = texture.loadFromFile(path);
    if (textureLoaded) {
        texture.setSmooth(false);
        frameW = fw; frameH = fh;
        sprite.setTexture(texture);           // привязать к НАШЕЙ texture
        sprite.setOrigin(fw / 2.f, fh / 2.f);
        sprite.setTextureRect(sf::IntRect(0, 0, fw, fh));
        sprite.setPosition(body.getPosition());
        // Масштаб: ENEMY_SPRITE_SCALE чтобы анимация была заметна
        float desired = body.getRadius() * C::ENEMY_SPRITE_SCALE;
        float scale   = desired / (float)fw;
        sprite.setScale(scale, scale);
    }
    return textureLoaded;
}

// ─── Анимация ────────────────────────────────────────────────
void Enemy::advanceAnim(float dt, bool moving) {
    if (!moving || !textureLoaded) {
        if (!moving && textureLoaded)
            sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
        return;
    }
    animTimer += dt;
    if (animTimer >= animSpeed) {
        animTimer -= animSpeed;
        animFrame = (animFrame + 1) % 4;
        sprite.setTextureRect(
            sf::IntRect(animFrame * frameW, 0, frameW, frameH));
    }
}

// ─── Обновление ──────────────────────────────────────────────
void Enemy::update(float dt, sf::Vector2f playerPos) {
    if (damageCooldown > 0.f) damageCooldown -= dt;

    sf::Vector2f dir = playerPos - body.getPosition();
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    bool moving = (len > 1.f);
    if (moving) dir /= len;

    body.move(dir * speed * dt);

    if (textureLoaded) {
        if (moving)
            aimAngle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;
        sprite.setPosition(body.getPosition());
        sprite.setRotation(aimAngle);
    }
    advanceAnim(dt, moving);
}

// ─── Урон ────────────────────────────────────────────────────
bool Enemy::takeDamage(float amount) {
    applyDamage(amount);
    if (textureLoaded) {
        float ratio = health / maxHealth;
        sf::Uint8 g = (sf::Uint8)(ratio * 200.f);
        sf::Uint8 b = (sf::Uint8)(ratio * 180.f);
        sprite.setColor(sf::Color(255, g, b, 230));
    }
    return health <= 0.f;
}

// ─── Цвет для частиц ─────────────────────────────────────────
sf::Color Enemy::typeColor() const {
    switch (type) {
        case EnemyType::BASIC: return sf::Color(220,  60,  60);
        case EnemyType::FAST:  return sf::Color(220, 150,   0);
        case EnemyType::TANK:  return sf::Color(130,  40, 200);
        case EnemyType::BOSS:  return sf::Color(255,   0,   0);
    }
    return sf::Color::White;
}

// ─── Полоска HP ──────────────────────────────────────────────
void Enemy::drawHealthBar(sf::RenderWindow& window) const {
    if (health >= maxHealth) return;
    float r    = body.getRadius();
    float barW = (type == EnemyType::BOSS) ? r*3.0f : r*2.4f;
    float barH = (type == EnemyType::BOSS) ? 9.f    : 5.f;
    sf::Vector2f pos = body.getPosition();
    float bx = pos.x - barW/2.f, by = pos.y - r - 14.f;

    sf::RectangleShape bg({barW, barH});
    bg.setPosition(bx, by);
    bg.setFillColor(sf::Color(60,60,60,200));
    window.draw(bg);

    float ratio = health / maxHealth;
    sf::RectangleShape fill({barW*ratio, barH});
    fill.setPosition(bx, by);
    if (type == EnemyType::BOSS) {
        sf::Uint8 rv = (sf::Uint8)(80 + 175*(1.f-ratio));
        fill.setFillColor(sf::Color(rv, 0, 180, 230));
    } else {
        sf::Uint8 g  = (sf::Uint8)(200.f*ratio);
        sf::Uint8 r2 = (sf::Uint8)(50.f + 200.f*(1.f-ratio));
        fill.setFillColor(sf::Color(r2, g, 30));
    }
    window.draw(fill);

    if (type == EnemyType::BOSS) {
        sf::RectangleShape border({barW, barH});
        border.setPosition(bx, by);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1.f);
        border.setOutlineColor(sf::Color(200,50,200,200));
        window.draw(border);
    }
}

// ─── Отрисовка ───────────────────────────────────────────────
void Enemy::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        // Ореол (аддитивный блend)
        sf::RenderStates glowState;
        glowState.blendMode = sf::BlendAdd;
        sf::Sprite halo = sprite;
        halo.setScale(sprite.getScale() * 1.6f);
        sf::Color hc = typeColor();
        hc.a = (type == EnemyType::BOSS) ? 80 : 45;
        halo.setColor(hc);
        // Хало тоже нужно перепривязать к texture (копия sprite!)
        halo.setTexture(texture);
        window.draw(halo, glowState);
        window.draw(sprite);
    } else {
        window.draw(body);
    }
    drawHealthBar(window);
}
