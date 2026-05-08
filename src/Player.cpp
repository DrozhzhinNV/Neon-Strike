#include "Player.h"
#include "Bullet.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

Player::Player()
    : Entity(C::PLAYER_MAX_HP),
      speed(C::PLAYER_SPEED), currentWeaponIdx(0), resources(0),
      speedMult(1.f), damageMult(1.f), maxHealthBonus(0.f)
{
    body.setPointCount(3);
    body.setPoint(0, { 20.f,   0.f});
    body.setPoint(1, {-14.f, -13.f});
    body.setPoint(2, {-14.f,  13.f});
    body.setOrigin(3.f, 0.f);
    body.setFillColor(sf::Color(50, 200, 255));
    body.setOutlineThickness(2.f);
    body.setOutlineColor(sf::Color(200, 240, 255));
    body.setPosition(C::MAP_W / 2.f, C::MAP_H / 2.f);

    gunDot.setRadius(3.f);
    gunDot.setOrigin(3.f, 3.f);
    gunDot.setFillColor(sf::Color(255, 255, 100));

    weapons.push_back(Weapon::makePistol());
}

void Player::handleInput(float dt, const sf::RenderWindow& window) {
    sf::Vector2f move{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move.x += 1.f;

    float len = std::sqrt(move.x * move.x + move.y * move.y);
    if (len > 0.f) move /= len;

    body.move(move * speed * speedMult * dt);
    clampToMap();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) switchWeapon(0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) switchWeapon(1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) switchWeapon(2);

    weapons[currentWeaponIdx].updateCooldown(dt);
    updateAim(window);
}

void Player::updateAim(const sf::RenderWindow& window) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
    sf::Vector2f dir = mouseWorld - body.getPosition();
    float angle = std::atan2(dir.y, dir.x) * 180.f / C::PI;
    body.setRotation(angle);

    float rad = angle * C::PI / 180.f;
    sf::Vector2f tip = body.getPosition()
                     + sf::Vector2f(std::cos(rad) * 23.f,
                                    std::sin(rad) * 23.f);
    gunDot.setPosition(tip);
}

std::vector<Bullet> Player::tryShoot(const sf::RenderWindow& window) {
    Weapon& w = weapons[currentWeaponIdx];
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) || !w.canFire())
        return {};

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
    sf::Vector2f dir = mouseWorld - body.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 1.f) return {};
    dir /= len;

    return w.fire(body.getPosition(), dir, damageMult);
}

void Player::takeDamage(float amount) {
    applyDamage(amount);
}

void Player::addResource(int amount) {
    resources += amount;
}

void Player::switchWeapon(int index) {
    if (index >= 0 && index < (int)weapons.size())
        currentWeaponIdx = index;
}

void Player::addWeapon(Weapon w) {
    for (auto& existing : weapons)
        if (existing.type == w.type) return;
    weapons.push_back(w);
}

void Player::clampToMap() {
    sf::Vector2f pos = body.getPosition();
    float r = C::PLAYER_RADIUS;
    pos.x = std::clamp(pos.x, r, C::MAP_W - r);
    pos.y = std::clamp(pos.y, r, C::MAP_H - r);
    body.setPosition(pos);
}

void Player::drawHealthBar(sf::RenderWindow& window) const {
    const float barW = 50.f, barH = 6.f;
    sf::Vector2f pos = body.getPosition();
    float bx = pos.x - barW / 2.f;
    float by = pos.y - 32.f;

    sf::RectangleShape bg({barW, barH});
    bg.setPosition(bx, by);
    bg.setFillColor(sf::Color(80, 0, 0));
    window.draw(bg);

    float total = getMaxHealth();
    float ratio = health / total;
    sf::RectangleShape fill({barW * ratio, barH});
    fill.setPosition(bx, by);
    fill.setFillColor(sf::Color(50, 210, 50));
    window.draw(fill);
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(body);
    window.draw(gunDot);
    drawHealthBar(window);
}