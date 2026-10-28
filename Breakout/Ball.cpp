#include "Ball.h"
#include "GameManager.h" // avoid cicular dependencies

Ball::Ball(sf::RenderWindow* window, float velocity, GameManager* gameManager)
    : _window(window), _velocity(velocity), _gameManager(gameManager),
    _timeWithPowerupEffect(0.f), _isFireBall(false), _isAlive(true), _direction({1,1})
{
    _sprite.setRadius(RADIUS);
    _sprite.setFillColor(sf::Color::Cyan);
    _sprite.setPosition(0, 300);

    _topRec.setSize(sf::Vector2f(_window->getSize().x, _window->getSize().y / 30.f));
    _topRec.setFillColor(sf::Color::Black);
    _topRec.setPosition(0.f, -20.f);

    _leftRec.setSize(sf::Vector2f(_window->getSize().x / 30.f, _window->getSize().y));
    _leftRec.setFillColor(sf::Color::Black);
    _leftRec.setPosition(-25.f, 0.f);

    _rightRec.setSize(sf::Vector2f(_window->getSize().x / 30.f, _window->getSize().y));
    _rightRec.setFillColor(sf::Color::Black);
    _rightRec.setPosition(990.f, 0.f);
}

Ball::~Ball()
{
}

void Ball::update(float dt)
{
    // check for powerup, tick down or correct
    if (_timeWithPowerupEffect > 0.f)
    {
        _timeWithPowerupEffect -= dt;
    }
    else
    {
        if (_velocity != VELOCITY + _acceleration)
            _velocity = VELOCITY + _acceleration;   // reset speed.
        else
        {
            setFireBall(0);    // disable fireball
            _sprite.setFillColor(sf::Color::Cyan);  // back to normal colour.
        }        
    }

    // Fireball effect
    if (_isFireBall)
    {
        // Flickering effect
        int flicker = rand() % 50 + 205; // Random value between 205 and 255
        _sprite.setFillColor(sf::Color(flicker, flicker / 2, 0)); // Orange flickering color
    }

    // Update position with a subtle floating-point error
    _sprite.move(_direction * _velocity * dt);

    // check bounds and bounce
    sf::Vector2f position = _sprite.getPosition();
    sf::Vector2u windowDimensions = _window->getSize();

    // bounce on walls
    if ((position.x >= windowDimensions.x - 2 * RADIUS && _direction.x > 0) || (position.x <= 0 && _direction.x < 0))
    {
        _direction.x *= -1;
        _sidetColourTimer = 0.1f;
        _acceleration += 5.f;
    }

    // bounce on ceiling
    if (position.y <= 0 && _direction.y < 0)
    {
        _direction.y *= -1;
        _topColourTimer = 0.1f;
        _acceleration += 5.f;
    }

    // lose life bounce
    if (position.y > windowDimensions.y)
    {
        _sprite.setPosition(0, 300);
        _direction = { 1, 1 };
        _gameManager->loseLife();
        _acceleration = 0;
    }

    // collision with paddle
    if (_sprite.getGlobalBounds().intersects(_gameManager->getPaddle()->getBounds()))
    {
        _direction.y *= -1; // Bounce vertically

        float paddlePositionProportion = (_sprite.getPosition().x - _gameManager->getPaddle()->getBounds().left) / _gameManager->getPaddle()->getBounds().width;
        _direction.x = paddlePositionProportion * 2.0f - 1.0f;

        // Adjust position to avoid getting stuck inside the paddle
        _sprite.setPosition(_sprite.getPosition().x, _gameManager->getPaddle()->getBounds().top - 2 * RADIUS);

        _gameManager->getPaddle()->setTimer();
        _acceleration += 5.f;
    }

    // collision with bricks
    int collisionResponse = _gameManager->getBrickManager()->checkCollision(_sprite, _direction);
    if (_isFireBall) return; // no collisisons when in fireBall mode.
    if (collisionResponse == 1)
    {
        _direction.x *= -1; // Bounce horizontally
    }
    else if (collisionResponse == 2)
    {
        _direction.y *= -1; // Bounce vertically
    }

    Trail trail;
    trail._position = sf::Vector2(_sprite.getPosition().x + RADIUS, _sprite.getPosition().y + RADIUS);
    trail._colour = sf::Color::Cyan;
    trail._lifetime = 0.5f;
    _trailVector.push_back(trail);

    for (auto &_tv : _trailVector) 
    {
        _tv._lifetime -= dt;
        if (_tv._lifetime > 0) {
            float alpha = (_tv._lifetime / 0.5f) * 255;
            _tv._colour.a = alpha;
        }
    }

    _trailVector.erase(
        std::remove_if(_trailVector.begin(), _trailVector.end(),
            [](const Trail& p) { return p._lifetime <= 0; }),
        _trailVector.end()
    );

    if (_topColourTimer > 0.f)
    {
        _topColourTimer -= dt;
        _topRec.setFillColor(sf::Color::Green);
    }
    else
    {
        _topRec.setFillColor(sf::Color::Black);
    }

    if (_sidetColourTimer > 0.f)
    {
        _sidetColourTimer -= dt;
        _leftRec.setFillColor(sf::Color::Green);
        _rightRec.setFillColor(sf::Color::Green);
    }
    else
    {
        _leftRec.setFillColor(sf::Color::Black);
        _rightRec.setFillColor(sf::Color::Black);
    }
}

void Ball::render()
{
    sf::VertexArray lines(sf::LinesStrip, _trailVector.size());
    for (int i = 0; i < _trailVector.size(); ++i) {
        lines[i].position = _trailVector[i]._position;
        lines[i].color = _trailVector[i]._colour;
    }
    _window->draw(lines);
    _window->draw(_sprite);
    _window->draw(_topRec);
    _window->draw(_rightRec);
    _window->draw(_leftRec);
}

void Ball::setVelocity(float coeff, float duration)
{
    _velocity = coeff * (VELOCITY + _acceleration);
    _timeWithPowerupEffect = duration;
}

void Ball::setFireBall(float duration)
{
    if (duration) 
    {
        _isFireBall = true;
        _timeWithPowerupEffect = duration;        
        return;
    }
    _isFireBall = false;
    _timeWithPowerupEffect = 0.f;    
}
