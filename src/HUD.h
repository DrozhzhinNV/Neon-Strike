#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Player;
class WaveManager;

class HUD {
public:
    explicit HUD(sf::RenderWindow& window);

    void draw(const Player& player,
              const WaveManager& wave,
              int score) const;

private:
    sf::RenderWindow& m_window;
    sf::Font font;
    bool fontLoaded;

    void drawHealthBar(float hp, float maxHp) const;
    void drawText(const std::string& text, sf::Vector2f pos,
                  unsigned int size,
                  sf::Color color = sf::Color::White) const;
};