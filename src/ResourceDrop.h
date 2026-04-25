#pragma once
#include <SFML/Graphics.hpp>


struct ResourceDrop {
    sf::CircleShape shape;
    int             amount; 
    bool            collected;

    ResourceDrop(sf::Vector2f pos, int amt);

    void draw(sf::RenderWindow& window) const;

    sf::Vector2f getPosition() const { return shape.getPosition(); }
    float        getRadius()   const { return shape.getRadius(); }
};
