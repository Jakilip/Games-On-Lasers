#include "Random.h"

#include <random>
#include <algorithm>
#include <chrono>

namespace GOL {

Random::Random() : Random(static_cast<unsigned>(
      std::chrono::steady_clock::now().time_since_epoch().count())) {}

Random::Random(unsigned seed) : randomEngine(seed), seed(seed) {}

int Random::getInt(int min, int max) {
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(randomEngine);
}

float Random::getFloat(float min, float max) {
    if (min > max) std::swap(min, max);
    if (min == max) return min;
    std::uniform_real_distribution<float> dist(min, max);
    return dist(randomEngine);
}

bool Random::getBool(float truthChance) {
    std::bernoulli_distribution dist(std::clamp(static_cast<double>(truthChance), 0.0, 1.0));
    return dist(randomEngine);
}

}