#pragma once
#include <random>
#include <chrono>

class Random {
private:
    std::mt19937 rng;

public:
    Random() {
        auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
    }

    int getInt(int max) {
        return std::uniform_int_distribution<int>(0, max - 1)(rng);
    }

    int getInt(int min, int max) {
        return std::uniform_int_distribution<int>(min, max)(rng);
    }

    float getFloat() {
        return std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
    }

    bool chance(int percent) {
        return getInt(100) < percent;
    }
};