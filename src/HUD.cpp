#include "HUD.h"
#include "Player.h"
#include "WaveManager.h"
#include "Constants.h"
#include <sstream>

// ============================================================
//  HUD.cpp
//  Разработчик Б.
// ============================================================

HUD::HUD(sf::RenderWindow& w) : window(w), fontLoaded(false) {
    fontLoaded = font.loadFromFile("assets/font.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
}

void HUD::drawText(const std::string& text, sf::Vector2f pos,
                   unsigned int size, sf::Color color) const
{
    if (!fontLoaded) return;
    sf::Text shadow(text, font, size);
    shadow.setPosition(pos + sf::Vector2f(2.f,2.f));
    shadow.setFillColor(sf::Color(0,0,0,180));
    window.draw(shadow);
    sf::Text label(text, font, size);
    label.setPosition(pos);
    label.setFillColor(color);
    window.draw(label);
}

void HUD::drawHealthBar(const Player& player) const {
    const float barW=200.f, barH=20.f;
    float x=20.f, y=(float)C::WINDOW_H - 50.f;

    sf::RectangleShape bg({barW,barH});
    bg.setPosition(x,y); bg.setFillColor(sf::Color(70,0,0,200));
    window.draw(bg);

    float ratio = player.getHealth() / player.getMaxHealth();
    sf::RectangleShape fill({barW*ratio,barH});
    fill.setPosition(x,y);
    sf::Uint8 g=(sf::Uint8)(200*ratio), r2=(sf::Uint8)(50+200*(1-ratio));
    fill.setFillColor(sf::Color(r2,g,30));
    window.draw(fill);

    // Рамка
    sf::RectangleShape border({barW,barH});
    border.setPosition(x,y);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(1.5f);
    border.setOutlineColor(sf::Color(160,160,160));
    window.draw(border);

    drawText(std::to_string((int)player.getHealth())+" / "+
             std::to_string((int)player.getMaxHealth()),
             {x+6.f, y+2.f}, 13);
}

void HUD::drawAmmoBar(const Player& player) const {
    const Weapon& w = player.getCurrentWeapon();
    float x=20.f, y=(float)C::WINDOW_H - 90.f;

    // Название оружия
    drawText(w.name, {x, y-18.f}, 16, sf::Color(200,220,255));

    if (w.reloading) {
        // Полоска перезарядки
        float ratio = 1.f - (w.reloadTimer / w.reloadTime);
        const float barW=140.f, barH=10.f;
        sf::RectangleShape bg({barW,barH});
        bg.setPosition(x, y); bg.setFillColor(sf::Color(60,40,0,200));
        window.draw(bg);
        sf::RectangleShape fill({barW*ratio,barH});
        fill.setPosition(x,y); fill.setFillColor(sf::Color(240,180,0,230));
        window.draw(fill);
        drawText("Reloading...", {x+barW+6.f, y-2.f}, 13,
                 sf::Color(240,180,0));
    } else {
        // Патроны в обойме — маленькие прямоугольники
        int mag = w.effectiveMagSize();
        float bw = 8.f, bh = 14.f, gap = 2.f;
        for (int i = 0; i < mag; ++i) {
            sf::RectangleShape bullet({bw,bh});
            bullet.setPosition(x + i*(bw+gap), y);
            bullet.setFillColor(i < w.currentAmmo
                ? sf::Color(240,200,40,230)
                : sf::Color(60,60,60,180));
            bullet.setOutlineThickness(0.5f);
            bullet.setOutlineColor(sf::Color(120,100,20,150));
            window.draw(bullet);
        }

        // Запас патронов
        drawText("/ " + std::to_string(w.totalAmmo),
                 {x + mag*(bw+gap) + 4.f, y}, 14,
                 sf::Color(160,160,180));
    }
}

void HUD::draw(const Player& player, const WaveManager& wave, int score) const {
    sf::View saved = window.getView();
    window.setView(window.getDefaultView());

    drawHealthBar(player);
    drawAmmoBar(player);

    // Оружие: клавиши
    if (player.getWeaponCount() > 1)
        drawText("[1][2][3] switch  [R] reload  [TAB] shop",
                 {20.f, (float)C::WINDOW_H - 110.f},
                 12, sf::Color(100,110,130));

    // Волна
    drawText("Wave " + std::to_string(wave.getCurrentWave()),
             {(float)C::WINDOW_W/2.f - 45.f, 10.f},
             26, sf::Color(255,220,80));

    // Очки и ресурсы
    drawText("Score: " + std::to_string(score),
             {(float)C::WINDOW_W - 190.f, 10.f}, 20);
    drawText("$  " + std::to_string(player.getResources()),
             {(float)C::WINDOW_W - 190.f, 35.f},
             18, sf::Color(0,220,170));

    // Нижняя строка
    drawText("WASD-move  LMB-shoot  ESC-exit",
             {10.f, (float)C::WINDOW_H - 16.f},
             12, sf::Color(80,80,100));

    window.setView(saved);
}
