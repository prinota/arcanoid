#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameState;
class Random;

class Bonus {
protected:
    sf::CircleShape shape;
    float speed;
    std::string message;

public:
    Bonus(float x, float y, float spd);
    virtual ~Bonus() = default;

    virtual void applyEffect(GameState& state, Random& rng,
        sf::RectangleShape& paddle) = 0;
    virtual int getType() const = 0;

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;
    bool isOffScreen() const;
    std::string getMessage() const { return message; }

protected:
    void setColor(sf::Color color) { shape.setFillColor(color); }
};

class ScoreBonus : public Bonus {
public:
    ScoreBonus(float x, float y);
    void applyEffect(GameState& state, Random& rng,
        sf::RectangleShape& paddle) override;
    int getType() const override { return 0; }
};

class PaddleSizeBonus : public Bonus {
public:
    PaddleSizeBonus(float x, float y);
    void applyEffect(GameState& state, Random& rng,
        sf::RectangleShape& paddle) override;
    int getType() const override { return 1; }
};

class BallSpeedBonus : public Bonus {
public:
    BallSpeedBonus(float x, float y);
    void applyEffect(GameState& state, Random& rng,
        sf::RectangleShape& paddle) override;
    int getType() const override { return 2; }
};

class ExtraLifeBonus : public Bonus {
public:
    ExtraLifeBonus(float x, float y);
    void applyEffect(GameState& state, Random& rng,
        sf::RectangleShape& paddle) override;
    int getType() const override { return 3; }
};

class RandomAngleBonus : public Bonus {
public:
    RandomAngleBonus(float x, float y);
    void applyEffect(GameState& state, Random& rng,
        sf::RectangleShape& paddle) override;
    int getType() const override { return 4; }
};