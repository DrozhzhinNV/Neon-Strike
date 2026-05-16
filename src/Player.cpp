#include "Player.h"
#include "Bullet.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

// ============================================================
//  Player.cpp
//  Разработчик А.
// ============================================================

Player::Player()
    : Entity(C::PLAYER_MAX_HP),
      speed(C::PLAYER_SPEED), currentWeaponIdx(0), resources(0)
{
    fallbackBody.setPointCount(3);
    fallbackBody.setPoint(0, { 20.f,  0.f});
    fallbackBody.setPoint(1, {-14.f,-13.f});
    fallbackBody.setPoint(2, {-14.f, 13.f});
    fallbackBody.setOrigin(3.f, 0.f);
    fallbackBody.setFillColor(sf::Color(50,200,255));
    fallbackBody.setOutlineThickness(2.f);
    fallbackBody.setOutlineColor(sf::Color(200,240,255));
    fallbackDot.setRadius(3.f);
    fallbackDot.setOrigin(3.f,3.f);
    fallbackDot.setFillColor(sf::Color(255,255,100));

    sprite.setPosition(C::MAP_W/2.f, C::MAP_H/2.f);
    fallbackBody.setPosition(C::MAP_W/2.f, C::MAP_H/2.f);

    weapons.push_back(Weapon::makePistol());
}

// ── Загрузка спрайт-листа ────────────────────────────────────
bool Player::loadTexture(const std::string& path) {
    textureLoaded = texture.loadFromFile(path);
    if (textureLoaded) {
        texture.setSmooth(false);
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, FRAME_W, FRAME_H));
        sprite.setOrigin(FRAME_W/2.f, FRAME_H/2.f);
        // Крупный масштаб — хорошо видно анимацию
        float desired = C::PLAYER_RADIUS * C::PLAYER_SPRITE_SCALE;
        sprite.setScale(desired/FRAME_W, desired/FRAME_W);
    }
    return textureLoaded;
}

void Player::advanceAnim(float dt) {
    if (!textureLoaded) return;
    if (!isMoving) {
        sprite.setTextureRect(sf::IntRect(0, 0, FRAME_W, FRAME_H));
        animFrame = 0; animTimer = 0.f;
        return;
    }
    animTimer += dt;
    if (animTimer >= animSpeed) {
        animTimer -= animSpeed;
        animFrame = (animFrame + 1) % 4;
        sprite.setTextureRect(
            sf::IntRect(animFrame * FRAME_W, 0, FRAME_W, FRAME_H));
    }
}

void Player::handleInput(float dt, const sf::RenderWindow& window) {
    sf::Vector2f move{0.f,0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move.x += 1.f;

    float len = std::sqrt(move.x*move.x + move.y*move.y);
    isMoving = (len > 0.f);
    if (isMoving) move /= len;

    sprite.move(move * speed * speedMult * dt);
    fallbackBody.move(move * speed * speedMult * dt);
    clampToMap();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) switchWeapon(0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) switchWeapon(1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) switchWeapon(2);

    // Ручная перезарядка — R
    static bool rWasPressed = false;
    bool rNow = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
    if (rNow && !rWasPressed) startReload();
    rWasPressed = rNow;

    weapons[currentWeaponIdx].updateCooldown(dt);
    if (hitFlashTimer > 0.f) hitFlashTimer -= dt;
    advanceAnim(dt);
    updateAim(window);
}

void Player::startReload() {
    weapons[currentWeaponIdx].startReload();
}

void Player::updateAim(const sf::RenderWindow& window) {
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    sf::Vector2f mw = window.mapPixelToCoords(mp);
    sf::Vector2f dir = mw - sprite.getPosition();
    float angle = std::atan2(dir.y, dir.x) * 180.f / C::PI;
    sprite.setRotation(angle);
    fallbackBody.setRotation(angle);
    float rad = angle * C::PI / 180.f;
    fallbackDot.setPosition(sprite.getPosition()
        + sf::Vector2f(std::cos(rad)*23.f, std::sin(rad)*23.f));
}

// ── Стрельба ─────────────────────────────────────────────────
std::vector<Bullet> Player::tryShoot(const sf::RenderWindow& window) {
    Weapon& w = weapons[currentWeaponIdx];
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) || !w.canFire())
        return {};
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    sf::Vector2f mw = window.mapPixelToCoords(mp);
    sf::Vector2f dir = mw - sprite.getPosition();
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (len < 1.f) return {};
    dir /= len;
    return w.fire(sprite.getPosition(), dir, damageMult);
}

void Player::takeDamage(float amount) { applyDamage(amount); hitFlashTimer = 0.14f; }
void Player::addResource(int amount)  { resources += amount; }
void Player::addAmmo(int amount) {
    weapons[currentWeaponIdx].addAmmo(amount);
}

void Player::switchWeapon(int i) {
    if (i >= 0 && i < (int)weapons.size()) currentWeaponIdx = i;
}
void Player::addWeapon(Weapon w) {
    for (auto& ex : weapons) if (ex.type == w.type) return;
    weapons.push_back(w);
}
void Player::clampToMap() {
    sf::Vector2f pos = sprite.getPosition();
    float r = C::PLAYER_RADIUS;
    pos.x = std::clamp(pos.x, r, C::MAP_W - r);
    pos.y = std::clamp(pos.y, r, C::MAP_H - r);
    sprite.setPosition(pos);
    fallbackBody.setPosition(pos);
}
void Player::setPosition(sf::Vector2f pos) {
    sprite.setPosition(pos);
    fallbackBody.setPosition(pos);
}


// ── Отрисовка HP ─────────────────────────────────────────────
void Player::drawHealthBar(sf::RenderWindow& window) const {
    const float barW=54.f, barH=6.f;
    sf::Vector2f pos = sprite.getPosition();
    float bx = pos.x - barW/2.f;
    float by = pos.y - C::PLAYER_RADIUS * C::PLAYER_SPRITE_SCALE/2.f - 10.f;

    sf::RectangleShape bg({barW,barH});
    bg.setPosition(bx,by); bg.setFillColor(sf::Color(80,0,0,200));
    window.draw(bg);
    float ratio = health / getMaxHealth();
    sf::RectangleShape fill({barW*ratio,barH});
    fill.setPosition(bx,by); fill.setFillColor(sf::Color(50,210,50));
    window.draw(fill);
}

// ── Отрисовка ────────────────────────────────────────────────
void Player::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        sf::Sprite ds = sprite;
        if (hitFlashTimer > 0.f) {
            float t = hitFlashTimer/0.14f;
            sf::Uint8 f = (sf::Uint8)(t*200.f);
            ds.setColor(sf::Color(255, 255-f/2, 255-f, 255));
        } else {
            ds.setColor(sf::Color::White);
        }
        sf::RenderStates glow; glow.blendMode = sf::BlendAdd;
        sf::Sprite halo = ds;
        halo.setScale(ds.getScale()*1.25f);
        halo.setColor(sf::Color(40,130,200,50));
        window.draw(halo, glow);
        window.draw(ds);
    } else {
        window.draw(fallbackBody);
        window.draw(fallbackDot);
    }
    drawHealthBar(window);
}

std::vector<Weapon>& Player::getWeapons() {
    return weapons;
}

const std::vector<Weapon>& Player::getWeapons() const {
    return weapons;
}
