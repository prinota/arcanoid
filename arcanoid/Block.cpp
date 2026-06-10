#include "Block.hpp"
#include "Bonus.hpp"
#include "GameState.hpp"
#include "Config.hpp"
#include <cmath>

Block::Block(float x, float y, BlockType blockType) : type(blockType) {
    shape.setSize(sf::Vector2f(Config::BLOCK_WIDTH, Config::BLOCK_HEIGHT));
    shape.setPosition(x, y);
}

void Block::draw(sf::RenderWindow& window) const {
    if (!destroyed) window.draw(shape);
}

sf::FloatRect Block::getBounds() const {
    return shape.getGlobalBounds();
}

sf::Vector2f Block::getPosition() const {
    return shape.getPosition();
}

UnbreakableBlock::UnbreakableBlock(float x, float y)
    : Block(x, y, BlockType::Unbreakable) {
    shape.setFillColor(sf::Color::White);
}

void UnbreakableBlock::onHit(GameState& state) {

}


RegularBlock::RegularBlock(float x, float y, int hp, sf::Color color)
    : Block(x, y, BlockType::Regular), health(hp), maxHealth(hp) {
    shape.setFillColor(color);
}

void RegularBlock::onHit(GameState& state) {
    health--;

    if (health <= 0) {
        destroy();
        state.addScore(getScoreValue());
    }
    else {
        float healthPercent = static_cast<float>(health) / maxHealth;
        if (healthPercent > 0.66f) {
            shape.setFillColor(sf::Color(255, 255, 100)); // Желтоватый
        }
        else if (healthPercent > 0.33f) {
            shape.setFillColor(sf::Color(255, 150, 50)); // Оранжевый
        }
        else {
            shape.setFillColor(sf::Color(255, 50, 50)); // Красный
        }
    }
}

bool RegularBlock::shouldSpawnBonus() const {
    return destroyed; 
}


SpeedBlock::SpeedBlock(float x, float y, float mod)
    : RegularBlock(x, y, 1, mod > 1 ? sf::Color::Magenta : sf::Color::Blue),
    speedModifier(mod) {
    type = mod > 1 ? BlockType::SpeedUp : BlockType::SpeedDown;
}

void SpeedBlock::onHit(GameState& state) {
    health--;

    if (health <= 0) {
        destroy();
        state.addScore(getScoreValue());
    }

    float currentSpeed = std::sqrt(
        state.ballVelocity.x * state.ballVelocity.x +
        state.ballVelocity.y * state.ballVelocity.y);

    if (isSpeedDown() && currentSpeed > Config::MIN_BALL_SPEED) {
        state.ballVelocity *= speedModifier;

        float newSpeed = std::sqrt(
            state.ballVelocity.x * state.ballVelocity.x +
            state.ballVelocity.y * state.ballVelocity.y);

        if (newSpeed < Config::MIN_BALL_SPEED) {
            float scale = Config::MIN_BALL_SPEED / newSpeed;
            state.ballVelocity *= scale;
        }
    }
    else if (isSpeedUp()) {
        state.ballVelocity *= speedModifier;

        float newSpeed = std::sqrt(
            state.ballVelocity.x * state.ballVelocity.x +
            state.ballVelocity.y * state.ballVelocity.y);

        constexpr float MAX_BALL_SPEED = Config::BALL_SPEED * 2.5f;
        if (newSpeed > MAX_BALL_SPEED) {
            float scale = MAX_BALL_SPEED / newSpeed;
            state.ballVelocity *= scale;
        }
    }
}

std::vector<std::unique_ptr<Block>> createBlocks(Random& rng) {
    std::vector<std::unique_ptr<Block>> blocks;

    sf::Color colors[] = {
        sf::Color::White,
        sf::Color::Red,
        sf::Color::Blue,
        sf::Color::Magenta,
        sf::Color::Yellow
    };

    for (int row = 0; row < Config::ROWS; row++) {
        for (int col = 0; col < Config::COLS; col++) {
            float x = Config::START_X + col * (Config::BLOCK_WIDTH + Config::BLOCK_PADDING);
            float y = Config::START_Y + row * (Config::BLOCK_HEIGHT + Config::BLOCK_PADDING);

            int type = rng.getInt(5);

            switch (type) {
            case 0:
                blocks.push_back(std::make_unique<UnbreakableBlock>(x, y));
                break;
            case 1:
                blocks.push_back(std::make_unique<RegularBlock>(x, y, 3, colors[type]));
                break;
            case 2:
                blocks.push_back(std::make_unique<SpeedBlock>(x, y, 0.7f));
                break;
            case 3:
                blocks.push_back(std::make_unique<SpeedBlock>(x, y, 1.3f));
                break;
            case 4:
                blocks.push_back(std::make_unique<RegularBlock>(x, y, 2, colors[type]));
                break;
            }
        }
    }

    return blocks;
}