#include "ResourceDrop.h"
#include "Constants.h"

ResourceDrop::ResourceDrop(sf::Vector2f pos, int amt)
    : amount(amt), collected(false)
{
    shape.setRadius(C::RESOURCE_RADIUS);
    shape.setOrigin(C::RESOURCE_RADIUS, C::RESOURCE_RADIUS);
    shape.setPosition(pos);
    shape.setFillColor(sf::Color(0, 220, 180));
    shape.setOutlineThickness(1.5f);
    shape.setOutlineColor(sf::Color(100, 255, 230));
}

void ResourceDrop::draw(sf::RenderWindow& window) const {
    if (!collected) window.draw(shape);
}
