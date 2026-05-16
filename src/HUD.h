#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Player;
class WaveManager;

// ============================================================
//  HUD.h — интерфейс: HP, волна, патроны, перезарядка.
//  Разработчик Б.
// ============================================================
class HUD {
public:
    HUD(sf::RenderWindow& window);
    void draw(const Player& player, const WaveManager& wave, int score) const;

private:
    sf::RenderWindow& window;
    sf::Font font;
    bool fontLoaded;

    void drawHealthBar(const Player& player) const;
    void drawAmmoBar(const Player& player) const;
    void drawText(const std::string& text, sf::Vector2f pos,
                  unsigned int size,
                  sf::Color color = sf::Color::White) const;
};
