#pragma once
#include <SFML/Graphics.hpp>
#include <string>

//  HUD.h — интерфейс поверх игры


class Player;
class WaveManager;

class HUD {
public:
    HUD();

    void draw(sf::RenderWindow& window,
              const Player&      player,
              const WaveManager& wave,
              int                score) const;

private:
    sf::Font font;
    bool     fontLoaded;

    void drawHealthBar(sf::RenderWindow& window, float hp, float maxHp) const;
    void drawText(sf::RenderWindow& window,
                  const std::string& text, sf::Vector2f pos,
                  unsigned int size,
                  sf::Color color = sf::Color::White) const;
};
