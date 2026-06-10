#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameState.hpp"
#include "Random.hpp"
#include "Bonus.hpp"
#include "Block.hpp"

class Block;
class Bonus;

enum class GameStatus {
    Playing,
    GameOver
};

class Game {
private:
    sf::RenderWindow window;
    Random rng;
    GameState state;
    GameStatus status = GameStatus::Playing;

    sf::RectangleShape paddle;
    sf::CircleShape ball;
    std::vector<std::unique_ptr<Block>> blocks;
    std::vector<std::unique_ptr<Bonus>> bonuses;

    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text messageText;
    float messageTimer = 0.f;
    float gameOverTimer = 0.f;

    void handleInput(float deltaTime);
    void updateBall();
    bool handleBlockCollision(Block& block);
    void updateBonuses(float deltaTime);
    void checkWinCondition();
    void checkBallLost();
    void updateUI(float deltaTime);
    void render();
    void showMessage(const std::string& text, float duration = 1.5f);
    void resetBall();
    void resetPaddle();

public:
    Game();
    void run();
};