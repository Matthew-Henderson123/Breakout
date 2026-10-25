#include "Brick.h"

Brick::Brick(float x, float y, float width, float height, int health)
    : _isDestroyed(false)
{
    _shape.setPosition(x, y);
    _shape.setSize(sf::Vector2f(width, height));
    _health = health;

    switch (_health)
    {
    case(1):
        _shape.setFillColor(sf::Color::Red);
        break;
    case(2):
        _shape.setFillColor(sf::Color::Yellow);
        break;
    case(3):
        _shape.setFillColor(sf::Color::Magenta);
        break;
    }
}

void Brick::render(sf::RenderWindow& window)
{
    if (!_isDestroyed) {
        window.draw(_shape);
    }
}

sf::FloatRect Brick::getBounds() const
{
    return _shape.getGlobalBounds();
}

int Brick::getHealth()
{
    return _health;
}

void Brick::setHealth()
{
    _health--;

    switch (_health)
    {
    case(1):
        _shape.setFillColor(sf::Color::Red);
        break;
    case(2):
        _shape.setFillColor(sf::Color::Yellow);
        break;
    case(3):
        _shape.setFillColor(sf::Color::Magenta);
        break;
    }
}

void Brick::setIsDestroyed()
{
    _isDestroyed = true;
}
