#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameState.hpp"
#include "Random.hpp"

class GameState;

enum class BlockType {
    Unbreakable,
    Regular,
    SpeedUp,
    SpeedDown
};

class Block {
protected:
    sf::RectangleShape shape;
    bool destroyed = false;
    BlockType type;

public:
    Block(float x, float y, BlockType blockType);
    virtual ~Block() = default;

    virtual void onHit(GameState& state) = 0;
    virtual bool isBreakable() const = 0;
    virtual int getScoreValue() const { return 0; }
    virtual float getSpeedModifier() const { return 1.0f; }
    virtual bool shouldSpawnBonus() const { return false; }

    void draw(sf::RenderWindow& window) const;
    bool isDestroyed() const { return destroyed; }
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    BlockType getType() const { return type; }

protected:
    void destroy() { destroyed = true; }
};

class UnbreakableBlock : public Block {
public:
    UnbreakableBlock(float x, float y);
    void onHit(GameState& state) override;
    bool isBreakable() const override { return false; }
};

class RegularBlock : public Block {
protected:
    int health;
    int maxHealth;

public:
    RegularBlock(float x, float y, int hp, sf::Color color);

    void onHit(GameState& state) override;
    bool isBreakable() const override { return true; }
    int getScoreValue() const override { return 100 * maxHealth; }
    bool shouldSpawnBonus() const override;
};

class SpeedBlock : public RegularBlock {
private:
    float speedModifier;

public:
    SpeedBlock(float x, float y, float mod);
    float getSpeedModifier() const override { return speedModifier; }
    bool isSpeedUp() const { return speedModifier > 1.f; }
    bool isSpeedDown() const { return speedModifier < 1.f; }

    void onHit(GameState& state) override;
};

std::vector<std::unique_ptr<Block>> createBlocks(Random& rng);