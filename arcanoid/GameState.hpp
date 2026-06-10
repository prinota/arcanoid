#pragma once
#include <SFML/System/Vector2.hpp>
#include "Config.hpp"

struct GameState {
    int score = 0;
    int lives = 3;
    bool extraLife = false;
    sf::Vector2f ballVelocity{ Config::BALL_SPEED, -Config::BALL_SPEED };

    void addScore(int points) { score += points; }
    void resetBall() {
        ballVelocity = sf::Vector2f(Config::BALL_SPEED, -Config::BALL_SPEED);
    }
};