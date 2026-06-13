#pragma once
#include <memory>
#include <vector>
#include "Bonus.hpp"

class Random;

class BonusFactory {
public:
    static std::unique_ptr<Bonus> createRandomBonus(float x, float y, Random& rng);

    static std::unique_ptr<Bonus> createBonus(int type, float x, float y);
};