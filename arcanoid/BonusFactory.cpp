#include "BonusFactory.hpp"
#include "Random.hpp"
#include "Config.hpp"

std::unique_ptr<Bonus> BonusFactory::createRandomBonus(float x, float y, Random& rng) {
    if (rng.chance(Config::BONUS_SPAWN_CHANCE)) {
        int type = rng.getInt(5);
        return createBonus(type, x, y);
    }
    return nullptr;
}

std::unique_ptr<Bonus> BonusFactory::createBonus(int type, float x, float y) {
    switch (type) {
    case 0: return std::make_unique<ScoreBonus>(x, y);
    case 1: return std::make_unique<PaddleSizeBonus>(x, y);
    case 2: return std::make_unique<BallSpeedBonus>(x, y);
    case 3: return std::make_unique<ExtraLifeBonus>(x, y);
    case 4: return std::make_unique<RandomAngleBonus>(x, y);
    default: return nullptr;
    }
}