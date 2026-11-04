#include <iostream>

#include "GOL.h"

/// PONG WITH GOL ///

int main() {

    GOL::Random rng;

    std::cout << "\nInts:\n";
    for(int i = 0; i < 10; i++) {
        std::cout << rng.getInt(0, 100) << "\n";
    }

    std::cout << "\nFloats:\n";
    for(int i = 0; i < 10; i++) {
        std::cout << rng.getFloat(0.0f, 1.0f) << "\n";
    }

    std::cout << "\nTruths:\n";
    for(int i = 0; i < 10; i++) {
        std::cout << rng.getBool(0.2f);
    }

    GOL::Projector projector;

    return 0;
};