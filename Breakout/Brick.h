#pragma once
#include <SFML/Graphics.hpp>

class Brick {
public:
    Brick(float x, float y, float width, float height, int health);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    int getHealth();
    void setHealth();
    void setIsDestroyed();

private:
    sf::RectangleShape _shape;
    bool _isDestroyed;
    int _health;
};