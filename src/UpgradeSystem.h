#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

class Player;

struct Upgrade {
    std::string name;
    std::string description;
    std::function<void(Player&)> apply;
};

class UpgradeSystem {
public:
    explicit UpgradeSystem(sf::RenderWindow& window);

    bool isActive() const { return active; }

    void show(Player& player);
    bool handleClick(Player& player);   // больше не требует позиции мыши

    void draw() const;                  // больше не требует окна

private:
    sf::RenderWindow& m_window;
    bool active;
    std::vector<Upgrade> offered;
    sf::Font font;
    bool fontLoaded;

    std::vector<Upgrade> buildAll(const Player& player);
    void drawCard(const Upgrade& u, sf::FloatRect rect, bool hovered) const;
};