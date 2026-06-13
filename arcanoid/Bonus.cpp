#include "Bonus.hpp"
#include "GameState.hpp"
#include "Random.hpp"
#include "Config.hpp"
#include <cmath>

Bonus::Bonus(float x, float y, float spd) : speed(spd) {
    shape.setRadius(Config::BONUS_RADIUS);
    shape.setPosition(x, y);
}

void Bonus::update(float deltaTime) {
    shape.move(0, speed * deltaTime * 60.f);
}

void Bonus::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

sf::FloatRect Bonus::getBounds() const {
    return shape.getGlobalBounds();
}

bool Bonus::isOffScreen() const {
    return shape.getPosition().y > Config::WINDOW_HEIGHT;
}

ScoreBonus::ScoreBonus(float x, float y) : Bonus(x, y, Config::BONUS_SPEED) {
    setColor(sf::Color::Cyan);
}

void ScoreBonus::applyEffect(GameState& state, Random& rng, sf::RectangleShape&) {
    if (rng.chance(80)) {
        state.addScore(100);
        message = "+100";
    }
    else {
        state.addScore(1000);
        message = "+1000";
    }
}

PaddleSizeBonus::PaddleSizeBonus(float x, float y) : Bonus(x, y, Config::BONUS_SPEED) {
    setColor(sf::Color::Green);
}

void PaddleSizeBonus::applyEffect(GameState&, Random& rng, sf::RectangleShape& paddle) {
    if (rng.chance(50)) {
        paddle.setSize(sf::Vector2f(Config::PADDLE_WIDTH * 1.5f, Config::PADDLE_HEIGHT));
        message = "Wide!";
    }
    else {
        paddle.setSize(sf::Vector2f(Config::PADDLE_WIDTH * 0.6f, Config::PADDLE_HEIGHT));
        message = "Narrow!";
    }
}

BallSpeedBonus::BallSpeedBonus(float x, float y) : Bonus(x, y, Config::BONUS_SPEED * 1.2f) {
    setColor(sf::Color::Magenta);
}

void BallSpeedBonus::applyEffect(GameState& state, Random& rng, sf::RectangleShape&) {
    if (rng.chance(50)) {
        state.ballVelocity *= 1.3f;
        message = "Fast!";
    }
    else {
        state.ballVelocity *= 0.7f;
        message = "Slow!";
    }
}

ExtraLifeBonus::ExtraLifeBonus(float x, float y) : Bonus(x, y, Config::BONUS_SPEED * 0.8f) {
    setColor(sf::Color::Red);
}

void ExtraLifeBonus::applyEffect(GameState& state, Random&, sf::RectangleShape&) {
    state.extraLife = true;
    state.lives++;
    message = "Extra Life!";
}

RandomAngleBonus::RandomAngleBonus(float x, float y) : Bonus(x, y, Config::BONUS_SPEED * 1.5f) {
    setColor(sf::Color::Yellow);
}

void RandomAngleBonus::applyEffect(GameState& state, Random& rng, sf::RectangleShape&) {
    float angle = static_cast<float>(rng.getInt(0, 359)) * 3.14159f / 180.f;
    float speed = std::sqrt(state.ballVelocity.x * state.ballVelocity.x +
        state.ballVelocity.y * state.ballVelocity.y);
    state.ballVelocity.x = speed * std::cos(angle);
    state.ballVelocity.y = -std::abs(speed * std::sin(angle));
    message = "Random!";
}