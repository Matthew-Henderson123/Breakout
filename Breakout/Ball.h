#pragma once
#include <SFML/Graphics.hpp>


class GameManager;  // forward declaration

struct Trail{
    sf::Vector2f _position;
    sf::Color _colour;
    float _lifetime;
};

class Ball {
public:
    Ball(sf::RenderWindow* window, float velocity, GameManager* gameManager);
    ~Ball();
    void update(float dt);
    void render();
    void setVelocity(float coeff, float duration);
    void setFireBall(float duration);

private:
    sf::CircleShape _sprite;
    sf::RectangleShape _topRec;
    sf::RectangleShape _leftRec;
    sf::RectangleShape _rightRec;
    sf::Vector2f _direction;
    sf::RenderWindow* _window;
    float _velocity;
    bool _isAlive;
    bool _isFireBall;
    float _timeWithPowerupEffect;

    float _acceleration = 0.0f;

    float _topColourTimer = 0.f;
    float _sidetColourTimer = 0.f;

    GameManager* _gameManager;  // Reference to the GameManager

    std::vector<Trail> _trailVector;

    static constexpr float RADIUS = 10.0f;      
    static constexpr float VELOCITY = 350.0f;   // for reference.
};

