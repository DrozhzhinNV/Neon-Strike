#include "HUD.h"
#include "Player.h"
#include "WaveManager.h"
#include "Constants.h"
#include <sstream>


//  HUD.cpp — отрисовка интерфейса


HUD::HUD() : fontLoaded(false) {
    if (font.loadFromFile("assets/font.ttf"))
        fontLoaded = true;
    else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        fontLoaded = true;
}

void HUD::drawText(sf::RenderWindow& window,
                   const std::string& text, sf::Vector2f pos,
                   unsigned int size, sf::Color color) const
{
    if (!fontLoaded) return;

    sf::Text shadow(text, font, size);
    shadow.setPosition(pos.x + 2.f, pos.y + 2.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(shadow);

    sf::Text label(text, font, size);
    label.setPosition(pos);
    label.setFillColor(color);
    window.draw(label);
}

void HUD::drawHealthBar(sf::RenderWindow& window,
                         float hp, float maxHp) const
{
    const float barW = 200.f, barH = 20.f;
    const float x = 20.f, y = (float)C::WINDOW_H - 45.f;


    sf::RectangleShape bg({barW, barH});
    bg.setPosition(x, y);
    bg.setFillColor(sf::Color(70, 0, 0));
    window.draw(bg);

    float ratio = hp / maxHp;
    sf::RectangleShape fill({barW * ratio, barH});
    fill.setPosition(x, y);
    auto g  = (sf::Uint8)(200.f * ratio);
    auto r2 = (sf::Uint8)(50.f + 200.f * (1.f - ratio));
    fill.setFillColor(sf::Color(r2, g, 30));
    window.draw(fill);

    sf::RectangleShape border({barW, barH});
    border.setPosition(x, y);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(1.5f);
    border.setOutlineColor(sf::Color(160, 160, 160));
    window.draw(border);


    drawText(window,
             std::to_string((int)hp) + " / " + std::to_string((int)maxHp),
             {x + 6.f, y + 2.f}, 14);
}

void HUD::draw(sf::RenderWindow& window,
               const Player& player,
               const WaveManager& wave,
               int score) const
{

    sf::View saved = window.getView();
    window.setView(window.getDefaultView());

    drawHealthBar(window, player.getHealth(), player.getMaxHealth());

    drawText(window,
             player.getCurrentWeapon().name,
             {20.f, (float)C::WINDOW_H - 70.f},
             18, sf::Color(200, 225, 255));

    if (player.getWeaponCount() > 1) {
        drawText(window, "[1][2][3] switch weapon",
                 {20.f, (float)C::WINDOW_H - 92.f},
                 13, sf::Color(110, 110, 130));
    }


    std::string waveStr = "Wave " + std::to_string(wave.getCurrentWave());
    drawText(window, waveStr,
             {(float)C::WINDOW_W / 2.f - 45.f, 10.f},
             26, sf::Color(255, 220, 80));

    drawText(window,
             "Score: " + std::to_string(score),
             {(float)C::WINDOW_W - 185.f, 10.f},
             20, sf::Color::White);

    drawText(window,
             "$  " + std::to_string(player.getResources()),
             {(float)C::WINDOW_W - 185.f, 38.f},
             18, sf::Color(0, 220, 180));

    drawText(window,
             "WASD - move    LMB - shoot    ESC - save & exit",
             {10.f, (float)C::WINDOW_H - 16.f},
             12, sf::Color(90, 90, 100));

    window.setView(saved);
}
