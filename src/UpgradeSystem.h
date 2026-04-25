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
    UpgradeSystem();

    bool isActive() const { return active; }


    void show(Player& player);
    bool handleClick(sf::Vector2f mousePos, Player& player);

    void draw(sf::RenderWindow& window) const;

private:
    bool                active;
    std::vector<Upgrade> offered;
    sf::Font             font;
    bool                 fontLoaded;

    std::vector<Upgrade> buildAll(const Player& player);


    void drawCard(sf::RenderWindow& window,
                  const Upgrade& u,
                  sf::FloatRect  rect,
                  bool           hovered) const;
};
