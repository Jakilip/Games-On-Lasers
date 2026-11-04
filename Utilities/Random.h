#pragma once
#include <random>

namespace GOL {

/// Simple class for generating random numbers. Default constructor picks a random seed.
class Random {
private:
    std::mt19937 randomEngine;
    unsigned seed;
public:

    /// @returns A random integer value from the range provided. Range is INCLUSIVE of min and max.
    int getInt(int min, int max);

    /// @returns A random float value from the range provided. Range is INCLUSIVE of min and max.
    float getFloat(float min, float max);

    /// @returns True or False, with the probability of True being returned expressed as a range from 0.f (0%) to 1.f (100%). Values out of this range will be clamped.
    bool getBool(float truthChance);

    Random();
    Random(unsigned seed);
};

}