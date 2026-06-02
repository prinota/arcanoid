#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PADDLE_WIDTH = 100.f;
const float PADDLE_HEIGHT = 15.f;
const float BALL_RADIUS = 8.f;
const float BALL_SPEED = 7.f;
const float PADDLE_SPEED = 5.f;

struct Block {
    sf::RectangleShape shape;
    bool destroyed = false;
    int health = 1;
    bool unbreakable = false;
    bool slows = false;
    bool speeds = false;
};

struct Bonus {
    sf::CircleShape shape;
    float speed = 3.f;
    int type = 0;
};

void generateBlocks(std::vector<Block>& blocks,
    const int rows, const int cols,
    const float blockWidth, const float blockHeight,
    const float blockPadding, const float startX, const float startY) {
    blocks.clear();

    sf::Color colors[] = {
        sf::Color::White,
        sf::Color::Red,
        sf::Color::Blue,
        sf::Color::Magenta,
        sf::Color::Yellow
    };

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Block block;
            block.shape.setSize(sf::Vector2f(blockWidth, blockHeight));

            int type = rand() % 5;
            block.shape.setFillColor(colors[type]);

            block.shape.setPosition(
                startX + col * (blockWidth + blockPadding),
                startY + row * (blockHeight + blockPadding)
            );

            switch (type) {
            case 0:
                block.unbreakable = true;
                block.health = 999;
                break;
            case 1:
                block.health = 3;
                break;
            case 4:
                block.health = 2;
                break;
            case 2:
                block.slows = true;
                break;
            case 3:
                block.speeds = true;
                break;
            }

            blocks.push_back(block);
        }
    }
}

int main() {
    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arkanoid");
    window.setFramerateLimit(60);

    sf::Clock clock;
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        if (!font.loadFromFile("C:/Windows/Fonts/consola.ttf")) {}
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    sf::Text messageText;
    messageText.setFont(font);
    messageText.setCharacterSize(36);
    messageText.setFillColor(sf::Color::Red);
    messageText.setString("");

    float messageTimer = 0.f;
    int score = 0;
    bool extraLife = false;

    sf::RectangleShape paddle(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    paddle.setFillColor(sf::Color::Green);
    paddle.setPosition(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - 50);

    sf::CircleShape ball(BALL_RADIUS);
    ball.setFillColor(sf::Color::White);
    ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 70);

    sf::Vector2f ballVelocity(BALL_SPEED, -BALL_SPEED);

    std::vector<Block> blocks;
    std::vector<Bonus> bonusDrops;
    const int rows = 5;
    const int cols = 10;
    const float blockWidth = 60.f;
    const float blockHeight = 20.f;
    const float blockPadding = 5.f;
    const float startX = 80.f;
    const float startY = 50.f;

    generateBlocks(blocks, rows, cols, blockWidth, blockHeight, blockPadding, startX, startY);

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        if (messageTimer > 0) {
            messageTimer -= deltaTime;
            if (messageTimer <= 0) {
                messageText.setString("");
            }
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            paddle.move(-PADDLE_SPEED, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            paddle.move(PADDLE_SPEED, 0);
        }

        sf::Vector2f paddlePos = paddle.getPosition();
        if (paddlePos.x < 0) paddle.setPosition(0, paddlePos.y);
        if (paddlePos.x > WINDOW_WIDTH - paddle.getSize().x)
            paddle.setPosition(WINDOW_WIDTH - paddle.getSize().x, paddlePos.y);

        ball.move(ballVelocity);

        if (ball.getPosition().x <= 0 ||
            ball.getPosition().x >= WINDOW_WIDTH - 2 * BALL_RADIUS) {
            ballVelocity.x = -ballVelocity.x;
        }
        if (ball.getPosition().y <= 0) {
            ballVelocity.y = -ballVelocity.y;
        }

        if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
            ballVelocity.y = -abs(ballVelocity.y);
            float hitPoint = (ball.getPosition().x - paddle.getPosition().x) / paddle.getSize().x;
            ballVelocity.x = (hitPoint - 0.5f) * BALL_SPEED * 2;
        }

        for (auto& block : blocks) {
            if (!block.destroyed &&
                ball.getGlobalBounds().intersects(block.shape.getGlobalBounds())) {

                if (block.unbreakable) {
                    ballVelocity.y = -ballVelocity.y;
                    break;
                }

                block.health--;

                if (block.health == 2) {
                    block.shape.setFillColor(sf::Color::Yellow);
                }
                else if (block.health == 1) {
                    block.shape.setFillColor(sf::Color::Green);
                }
                else if (block.health <= 0) {
                    block.destroyed = true;
                }

                if (block.slows) {
                    float currentSpeed = sqrt(ballVelocity.x * ballVelocity.x +
                        ballVelocity.y * ballVelocity.y);
                    if (currentSpeed > 2.f) {
                        ballVelocity *= 0.7f;
                    }
                }

                if (block.speeds) {
                    ballVelocity *= 1.3f;
                }

                ballVelocity.y = -ballVelocity.y;
                score += 100;

                if (rand() % 100 < 30) {
                    Bonus bonus;
                    bonus.shape.setRadius(10.f);
                    bonus.type = rand() % 5;

                    switch (bonus.type) {
                    case 0: bonus.shape.setFillColor(sf::Color::Cyan); break;
                    case 1: bonus.shape.setFillColor(sf::Color::Green); break;
                    case 2: bonus.shape.setFillColor(sf::Color::Magenta); break;
                    case 3: bonus.shape.setFillColor(sf::Color::Red); break;
                    case 4: bonus.shape.setFillColor(sf::Color::Yellow); break;
                    }

                    bonus.shape.setPosition(
                        block.shape.getPosition().x + blockWidth / 2 - 10.f,
                        block.shape.getPosition().y + blockHeight
                    );
                    bonusDrops.push_back(bonus);
                }
                break;
            }
        }

        bool allDestroyed = true;
        for (const auto& block : blocks) {
            if (!block.destroyed && !block.unbreakable) {
                allDestroyed = false;
                break;
            }
        }

        if (allDestroyed) {
            score += 5000;
            bonusDrops.clear();
            extraLife = false;
            paddle.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
            generateBlocks(blocks, rows, cols, blockWidth, blockHeight, blockPadding, startX, startY);

            ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 70);
            ballVelocity = sf::Vector2f(BALL_SPEED * 0.7f, -BALL_SPEED);

            messageText.setString("+5000");
            messageText.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 18);
            messageTimer = 1.5f;
        }

        if (ball.getPosition().y > WINDOW_HEIGHT) {
            if (extraLife) {
                extraLife = false;
                ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 70);
                ballVelocity = sf::Vector2f(BALL_SPEED * 0.7f, -BALL_SPEED);
                messageText.setString("Extra Life!");
                messageText.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 18);
                messageTimer = 1.5f;
            }
            else {
                ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 70);
                ballVelocity = sf::Vector2f(BALL_SPEED * 0.7f, -BALL_SPEED);
                score -= 1000;
                messageText.setString("-1000");
                messageText.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 18);
                messageTimer = 1.5f;
                bonusDrops.clear();
                extraLife = false;
                paddle.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
            }
        }

        scoreText.setString("Score: " + std::to_string(score));

        for (auto& bonus : bonusDrops) {
            bonus.shape.move(0, bonus.speed);
        }

        for (auto it = bonusDrops.begin(); it != bonusDrops.end(); ) {
            if (it->shape.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
                switch (it->type) {
                case 0:
                    if (rand() % 100 < 80) {
                        score += 100;
                        messageText.setString("+100");
                    }
                    else {
                        score += 1000;
                        messageText.setString("+1000");
                    }
                    messageText.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 18);
                    messageTimer = 1.0f;
                    break;

                case 1:
                    if (rand() % 2 == 0) {
                        paddle.setSize(sf::Vector2f(PADDLE_WIDTH * 1.5f, PADDLE_HEIGHT));
                        messageText.setString("Wide!");
                    }
                    else {
                        paddle.setSize(sf::Vector2f(PADDLE_WIDTH * 0.6f, PADDLE_HEIGHT));
                        messageText.setString("Narrow!");
                    }
                    messageText.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 18);
                    messageTimer = 1.0f;
                    break;

                case 2: {
                    float currentSpeed = sqrt(ballVelocity.x * ballVelocity.x +
                        ballVelocity.y * ballVelocity.y);
                    if (rand() % 2 == 0) {
                        ballVelocity *= 1.3f;
                        messageText.setString("Fast!");
                    }
                    else {
                        ballVelocity *= 0.7f;
                        messageText.setString("Slow!");
                    }
                    messageText.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 18);
                    messageTimer = 1.0f;
                    break;
                }

                case 3:
                    extraLife = true;
                    messageText.setString("Extra Life!");
                    messageText.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 18);
                    messageTimer = 1.5f;
                    break;

                case 4: {
                    float angle = (float)(rand() % 360) * 3.14159f / 180.f;
                    float speed = sqrt(ballVelocity.x * ballVelocity.x +
                        ballVelocity.y * ballVelocity.y);
                    ballVelocity.x = speed * cos(angle);
                    ballVelocity.y = -abs(speed * sin(angle));
                    messageText.setString("Random!");
                    messageText.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 18);
                    messageTimer = 1.0f;
                    break;
                }
                }
                it = bonusDrops.erase(it);
            }
            else if (it->shape.getPosition().y > WINDOW_HEIGHT) {
                it = bonusDrops.erase(it);
            }
            else {
                ++it;
            }
        }

        window.clear(sf::Color::Black);
        window.draw(paddle);
        window.draw(ball);

        for (const auto& bonus : bonusDrops) {
            window.draw(bonus.shape);
        }

        for (const auto& block : blocks) {
            if (!block.destroyed) {
                window.draw(block.shape);
            }
        }

        window.draw(scoreText);
        window.draw(messageText);

        window.display();
    }

    return 0;
}