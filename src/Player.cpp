#include "Player.h"
#include "Bullet.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

// ============================================================
//  Player.cpp — управление, отрисовка, стрельба.
//  Разработчик А.
// ============================================================

Player::Player()
: Entity(C::PLAYER_MAX_HP),
  speed(C::PLAYER_SPEED), currentWeaponIdx(0), resources(0),
  speedMult(1.f), damageMult(1.f), maxHealthBonus(0.f),
  hitFlashTimer(0.f)
{
    // --- Запасная геометрия ---
    fallbackBody.setPointCount(3);
    fallbackBody.setPoint(0, { 20.f,   0.f});
    fallbackBody.setPoint(1, {-14.f, -13.f});
    fallbackBody.setPoint(2, {-14.f,  13.f});
    fallbackBody.setOrigin(3.f, 0.f);
    fallbackBody.setFillColor(sf::Color(50, 200, 255));
    fallbackBody.setOutlineThickness(2.f);
    fallbackBody.setOutlineColor(sf::Color(200, 240, 255));
    fallbackDot.setRadius(3.f);
    fallbackDot.setOrigin(3.f, 3.f);
    fallbackDot.setFillColor(sf::Color(255, 255, 100));

    sprite.setPosition(C::MAP_W / 2.f, C::MAP_H / 2.f);
    fallbackBody.setPosition(C::MAP_W / 2.f, C::MAP_H / 2.f);
    weapons.push_back(Weapon::makePistol());
}

bool Player::loadTexture(const std::string& path) {
    textureLoaded = texture.loadFromFile(path);
    if (textureLoaded) {
        texture.setSmooth(true);
        sprite.setTexture(texture);
        auto sz = texture.getSize();
        sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
        float desired = C::PLAYER_RADIUS * 2.4f;
        float scale   = desired / sz.x;
        sprite.setScale(scale, scale);
    }
    return textureLoaded;
}

void Player::handleInput(float dt, const sf::RenderWindow& window) {
    sf::Vector2f move{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move.x += 1.f;

    float len = std::sqrt(move.x * move.x + move.y * move.y);
    if (len > 0.f) move /= len;

    sf::Vector2f delta = move * speed * speedMult * dt;
    sprite.move(delta);
    fallbackBody.move(delta);
    clampToMap();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) switchWeapon(0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) switchWeapon(1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) switchWeapon(2);

    weapons[currentWeaponIdx].updateCooldown(dt);
    if (hitFlashTimer > 0.f) hitFlashTimer -= dt;
    updateAim(window);
}

void Player::updateAim(const sf::RenderWindow& window) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f dir = mouseWorld - pos;
    float angle = std::atan2(dir.y, dir.x) * 180.f / C::PI;

    sprite.setRotation(angle);
    fallbackBody.setRotation(angle);

    float rad = angle * C::PI / 180.f;
    sf::Vector2f tip = pos + sf::Vector2f(std::cos(rad) * 23.f, std::sin(rad) * 23.f);
    fallbackDot.setPosition(tip);
}

std::vector<Bullet> Player::tryShoot(const sf::RenderWindow& window) {
    Weapon& w = weapons[currentWeaponIdx];
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) || !w.canFire()) return {};

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
    sf::Vector2f dir = mouseWorld - sprite.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 1.f) return {};
    dir /= len;

    return w.fire(sprite.getPosition(), dir, damageMult);
}

void Player::takeDamage(float amount) {
    applyDamage(amount);
    hitFlashTimer = 0.12f;
}
void Player::addResource(int amount) { resources += amount; }
void Player::switchWeapon(int index) {
    if (index >= 0 && index < (int)weapons.size()) currentWeaponIdx = index;
}
void Player::addWeapon(Weapon w) {
    for (auto& ex : weapons) if (ex.type == w.type) return;
    weapons.push_back(w);
}
void Player::clampToMap() {
    sf::Vector2f pos = sprite.getPosition();
    float r = C::PLAYER_RADIUS;
    pos.x = std::clamp(pos.x, r, (float)C::MAP_W - r);
    pos.y = std::clamp(pos.y, r, (float)C::MAP_H - r);
    sprite.setPosition(pos);
    fallbackBody.setPosition(pos);
}

void Player::drawHealthBar(sf::RenderWindow& window) const {
    const float barW = 50.f, barH = 6.f;
    sf::Vector2f pos = sprite.getPosition();
    float bx = pos.x - barW / 2.f;
    float by = pos.y - 32.f;

    sf::RectangleShape bg({barW, barH});
    bg.setPosition(bx, by);
    bg.setFillColor(sf::Color(80, 0, 0, 180));
    window.draw(bg);

    float total = getMaxHealth();
    float ratio = health / total;
    sf::RectangleShape fill({barW * ratio, barH});
    fill.setPosition(bx, by);
    fill.setFillColor(sf::Color(50, 210, 50));
    window.draw(fill);
}

void Player::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        // Локальная копия спрайта для безопасного изменения цвета
        sf::Sprite drawSprite = sprite;
        if (hitFlashTimer > 0.f) {
            float t = hitFlashTimer / 0.12f;
            sf::Uint8 flash = static_cast<sf::Uint8>(t * 200.f);
            drawSprite.setColor(sf::Color(255, 255 - flash/2, 255 - flash, 255));
        } else {
            drawSprite.setColor(sf::Color::White);
        }

        // Ореол (с блендом Add)
        sf::RenderStates glow;
        glow.blendMode = sf::BlendAdd;
        sf::Sprite halo = drawSprite;
        halo.setScale(drawSprite.getScale() * 1.35f);
        halo.setColor(sf::Color(40, 130, 200, 60));
        window.draw(halo, glow);

        window.draw(drawSprite);
    } else {
        window.draw(fallbackBody);
        window.draw(fallbackDot);
    }
    drawHealthBar(window);
}