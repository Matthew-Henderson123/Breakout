#include "GameManager.h"
#include "Ball.h"
#include "PowerupManager.h"
#include <iostream>

GameManager::GameManager(sf::RenderWindow* window)
    : _window(window), _paddle(nullptr), _ball(nullptr), _brickManager(nullptr), _powerupManager(nullptr),
    _messagingSystem(nullptr), _ui(nullptr), _pause(false), _time(0.f), _lives(3), _pauseHold(0.f), _levelComplete(false),
    _powerupInEffect({ none,0.f }), _timeLastPowerupSpawned(0.f)
{
    _font.loadFromFile("font/montS.ttf");
    _masterText.setFont(_font);
    _masterText.setPosition(50, 400);
    _masterText.setCharacterSize(48);
    _masterText.setFillColor(sf::Color::Yellow);

    _scoreText.setFont(_font);
    _scoreText.setPosition(200, 5);
    _scoreText.setCharacterSize(48);
    _scoreText.setFillColor(sf::Color::Yellow);

    _solidBar.setSize(sf::Vector2f(200.f, 30.f));
    _solidBar.setFillColor(sf::Color::Red);
    _solidBar.setPosition(500.f, 20.f);

    _progressBar.setSize(sf::Vector2f(200.f, 30.f));
    _progressBar.setFillColor(sf::Color::Yellow);
    _progressBar.setPosition(500.f, 20.f);

    _view = _window->getDefaultView();
    _originalCentre = _view.getCenter();

    _bottomRec.setSize(sf::Vector2f(_window->getSize().x, _window->getSize().y / 8.f));
    _bottomRec.setFillColor(sf::Color::Black);
    _bottomRec.setPosition(0.f, 780.f);
}

void GameManager::initialize()
{
    _paddle = new Paddle(_window);
    _brickManager = new BrickManager(_window, this);
    _messagingSystem = new MessagingSystem(_window);
    _ball = new Ball(_window, 400.0f, this); 
    _powerupManager = new PowerupManager(_window, _paddle, _ball);

    _lives = 3;
    _ui = new UI(_window, _lives, this);

    // Create bricks
    _brickManager->createBricks(5, 10, 80.0f, 30.0f, 5.0f);

    _masterText.setString("");
    _scoreText.setString("0");
    _score = 0;
    _powerupInEffect.second = 0;
}

void GameManager::update(float dt)
{
    _powerupInEffect = _powerupManager->getPowerupInEffect();
    _ui->updatePowerupText(_powerupInEffect);
    _powerupInEffect.second -= dt;

    if (_powerupInEffect.second > 0)
    {
        _powerTimePerc = _powerupInEffect.second / _maxPowerTime;
        _progressBar.setSize(sf::Vector2f((200.f * _powerTimePerc), 30.f));
    }
    else
    {
        _progressBar.setSize(sf::Vector2f(200.f, 30.f));
    }
    

    if (_lives <= 0)
    {
        _masterText.setString("Game over.");
        if (_restartCounter >= 0)
        {
            _restartCounter -= dt;
        }
        else
        {
            initialize();
            _shakeDuration = 0.f;
            _lives = 3;
        }
        return;
    }
    if (_levelComplete)
    {
        _masterText.setString("Level completed.");
        if (_restartCounter >= 0)
        {
            _restartCounter -= dt;
        }
        else
        {
            initialize();
            _levelComplete = false;
        }
        return;
    }
    // pause and pause handling
    if (_pauseHold > 0.f) _pauseHold -= dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
    {
        if (!_pause && _pauseHold <= 0.f)
        {
            _pause = true;
            _masterText.setString("paused.");
            _pauseHold = PAUSE_TIME_BUFFER;
        }
        if (_pause && _pauseHold <= 0.f)
        {
            _pause = false;
            _masterText.setString("");
            _pauseHold = PAUSE_TIME_BUFFER;
        }
    }
    if (_pause)
    {
        return;
    }

    // timer.
    _time += dt;


    if (_time > _timeLastPowerupSpawned + POWERUP_FREQUENCY && rand()%700 == 0)      // TODO parameterise
    {
        _powerupManager->spawnPowerup();
        _timeLastPowerupSpawned = _time;
    }

    // move paddle
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) _paddle->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) _paddle->moveLeft(dt);

    // update everything 
    _paddle->update(dt);
    _ball->update(dt);
    _powerupManager->update(dt);

    if (_shakeDuration > 0.0f)
    {
        _shakeDuration -= dt;

        int value = (std::rand() % 2);

        if (value == 0)
        {
            value = -1;
        }

        float offsetX = value * _shakeStrength;
        float offsetY = value * _shakeStrength;

        _view.setCenter(_originalCentre + sf::Vector2f(offsetX, offsetY));
        _window->setView(_view);
        _bottomRec.setFillColor(sf::Color::Red);
    }
    else 
    {
        _view.setCenter(_originalCentre);
        _window->setView(_view);
        _bottomRec.setFillColor(sf::Color::Black);
    }
}

void GameManager::loseLife()
{
    _lives--;
    _ui->lifeLost(_lives);

    // TODO screen shake.
    _shakeDuration = 0.3f;

    if (_lives == 0)
    {
        _restartCounter = 3.0f;
    }
}

void GameManager::render()
{
    if (_powerupInEffect.second > 0)
    {
        _window->clear(sf::Color(((std::sin(_time) * 0.5 + 0.5) * 255), ((std::sin(_time + 2) * 0.5 + 0.5) * 225), ((std::sin(_time + 4) * 0.5 + 0.5) * 225)));
    }

    if (_progressBar.getSize().x != 200)
    {
        _window->draw(_solidBar);
        _window->draw(_progressBar);
    }

    _paddle->render();
    _ball->render();
    _brickManager->render();
    _powerupManager->render();
    _window->draw(_masterText);
    _window->draw(_scoreText);
    _ui->render();
    _window->draw(_bottomRec);
}

void GameManager::levelComplete()
{
    _levelComplete = true;
    _restartCounter = 3.f;
}

sf::RenderWindow* GameManager::getWindow() const { return _window; }
UI* GameManager::getUI() const { return _ui; }

void GameManager::addScore()
{
    _score++;
    _scoreText.setString(std::to_string(_score));
}

Paddle* GameManager::getPaddle() const { return _paddle; }
BrickManager* GameManager::getBrickManager() const { return _brickManager; }
PowerupManager* GameManager::getPowerupManager() const { return _powerupManager; }
