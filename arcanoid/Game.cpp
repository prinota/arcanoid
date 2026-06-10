#include "Game.hpp"
#include "Block.hpp"
#include "Bonus.hpp"
#include "BonusFactory.hpp"
#include "Config.hpp"
#include <cmath>

Game::Game()
    : window(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), "Arkanoid")
{
    window.setFramerateLimit(60);

    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/consola.ttf");
    }

    paddle.setSize(sf::Vector2f(Config::PADDLE_WIDTH, Config::PADDLE_HEIGHT));
    paddle.setFillColor(sf::Color::Green);
    resetPaddle();

    ball.setRadius(Config::BALL_RADIUS);
    ball.setFillColor(sf::Color::White);
    resetBall();

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(Config::WINDOW_WIDTH - 150, 10);

    messageText.setFont(font);
    messageText.setCharacterSize(36);
    messageText.setFillColor(sf::Color::Red);

    blocks = createBlocks(rng);
}

void Game::resetPaddle() {
    paddle.setSize(sf::Vector2f(Config::PADDLE_WIDTH, Config::PADDLE_HEIGHT));
    paddle.setPosition(Config::WINDOW_WIDTH / 2 - Config::PADDLE_WIDTH / 2,
        Config::WINDOW_HEIGHT - 50);
}

void Game::resetBall() {
    ball.setPosition(Config::WINDOW_WIDTH / 2, Config::WINDOW_HEIGHT - 70);
    state.resetBall();
}

void Game::showMessage(const std::string& text, float duration) {
    messageText.setString(text);
    sf::FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    messageText.setPosition(Config::WINDOW_WIDTH / 2.0f, Config::WINDOW_HEIGHT / 2.0f);
    messageTimer = duration;
}

void Game::handleInput(float deltaTime) {
    bool moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D);

    float moveSpeed = Config::PADDLE_SPEED * deltaTime * 60.f;

    if (moveLeft) {
        paddle.move(-moveSpeed, 0);
    }
    if (moveRight) {
        paddle.move(moveSpeed, 0);
    }

    sf::Vector2f pos = paddle.getPosition();
    if (pos.x < 0) paddle.setPosition(0, pos.y);
    if (pos.x > Config::WINDOW_WIDTH - paddle.getSize().x) {
        paddle.setPosition(Config::WINDOW_WIDTH - paddle.getSize().x, pos.y);
    }
}

void Game::updateBall() {
    ball.move(state.ballVelocity);

    if (ball.getPosition().x <= 0) {
        ball.setPosition(0, ball.getPosition().y);
        state.ballVelocity.x = std::abs(state.ballVelocity.x);
    }
    if (ball.getPosition().x >= Config::WINDOW_WIDTH - 2 * Config::BALL_RADIUS) {
        ball.setPosition(Config::WINDOW_WIDTH - 2 * Config::BALL_RADIUS, ball.getPosition().y);
        state.ballVelocity.x = -std::abs(state.ballVelocity.x);
    }
    if (ball.getPosition().y <= 0) {
        ball.setPosition(ball.getPosition().x, 0);
        state.ballVelocity.y = std::abs(state.ballVelocity.y);
    }

    if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
        ball.setPosition(ball.getPosition().x,
            paddle.getPosition().y - 2 * Config::BALL_RADIUS);
        state.ballVelocity.y = -std::abs(state.ballVelocity.y);

        float hitPoint = (ball.getPosition().x + Config::BALL_RADIUS - paddle.getPosition().x)
            / paddle.getSize().x;
        state.ballVelocity.x = (hitPoint - 0.5f) * Config::BALL_SPEED * Config::BALL_SPEED_MULTIPLIER;

        float currentSpeed = std::sqrt(
            state.ballVelocity.x * state.ballVelocity.x +
            state.ballVelocity.y * state.ballVelocity.y);
        if (currentSpeed > 0) {
            float speedScale = Config::BALL_SPEED / currentSpeed;
            state.ballVelocity.x *= speedScale;
            state.ballVelocity.y *= speedScale;
        }
    }
}

bool Game::handleBlockCollision(Block& block) {
    if (!block.isDestroyed() &&
        ball.getGlobalBounds().intersects(block.getBounds())) {

        state.ballVelocity.y = -state.ballVelocity.y;
        block.onHit(state);

        if (block.shouldSpawnBonus()) {
            float bx = block.getPosition().x + Config::BLOCK_WIDTH / 2 - Config::BONUS_RADIUS;
            float by = block.getPosition().y + Config::BLOCK_HEIGHT;

            auto bonus = BonusFactory::createRandomBonus(bx, by, rng);
            if (bonus) {
                bonuses.push_back(std::move(bonus));
            }
        }
        return true;
    }
    return false;
}

void Game::updateBonuses(float deltaTime) {
    for (auto& bonus : bonuses) {
        bonus->update(deltaTime);
    }

    for (auto it = bonuses.begin(); it != bonuses.end();) {
        bool removed = false;

        if ((*it)->getBounds().intersects(paddle.getGlobalBounds())) {
            (*it)->applyEffect(state, rng, paddle);

            std::string msg = (*it)->getMessage();
            if (!msg.empty()) {
                showMessage(msg, 1.0f);
            }

            removed = true;
        }
        else if ((*it)->isOffScreen()) {
            removed = true;
        }

        if (removed) {
            it = bonuses.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Game::checkWinCondition() {
    bool allBreakableDestroyed = true;
    for (auto& block : blocks) {
        if (!block->isDestroyed() && block->isBreakable()) {
            allBreakableDestroyed = false;
            break;
        }
    }

    if (allBreakableDestroyed) {
        state.addScore(5000);
        showMessage("Level Complete! +5000", 2.0f);

        bonuses.clear();
        resetPaddle();
        blocks = createBlocks(rng);
        resetBall();
    }
}

void Game::checkBallLost() {
    if (ball.getPosition().y > Config::WINDOW_HEIGHT) {
        if (state.extraLife) {
            state.extraLife = false;
            showMessage("Extra Life Saved You!", 1.5f);
        }
        else {
            state.lives--;
            if (state.lives <= 0) {
                status = GameStatus::GameOver;
                gameOverTimer = 3.0f;
                showMessage("Game Over!", 3.0f);
                return;
            }
            else {
                showMessage("Life Lost!", 1.5f);
                bonuses.clear();
                resetPaddle();
            }
        }

        resetBall();
    }
}

void Game::updateUI(float deltaTime) {
    scoreText.setString("Score: " + std::to_string(state.score));
    livesText.setString("Lives: " + std::to_string(state.lives));

    if (messageTimer > 0) {
        messageTimer -= deltaTime;
        if (messageTimer <= 0) {
            messageText.setString("");
        }
    }
}

void Game::render() {
    window.clear(sf::Color::Black);

    window.draw(paddle);
    window.draw(ball);

    for (const auto& bonus : bonuses) {
        bonus->draw(window);
    }

    for (const auto& block : blocks) {
        block->draw(window);
    }

    window.draw(scoreText);
    window.draw(livesText);
    window.draw(messageText);

    window.display();
}

void Game::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::Space &&
                    status == GameStatus::GameOver) {
                    state.score = 0;
                    state.lives = 3;
                    state.extraLife = false;
                    blocks = createBlocks(rng);
                    bonuses.clear();
                    resetPaddle();
                    resetBall();
                    status = GameStatus::Playing;
                    messageText.setString("");
                }
            }
        }

        if (status == GameStatus::GameOver) {
            gameOverTimer -= deltaTime;
            if (gameOverTimer <= 0) {
                messageText.setString("Press Space to restart");
            }
            render();
            continue;
        }

        handleInput(deltaTime);
        updateBall();

        for (auto& block : blocks) {
            if (handleBlockCollision(*block)) {
                break;
            }
        }

        updateBonuses(deltaTime);
        checkWinCondition();
        checkBallLost();
        updateUI(deltaTime);
        render();
    }
}