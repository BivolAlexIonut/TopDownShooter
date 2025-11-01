#include <random>
#include "RandomGenerator.h"

namespace {
    std::random_device rd;
    std::mt19937 generator(rd());
}

float RandomGenerator::getFloat(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}