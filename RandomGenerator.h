#pragma once
#include <random>
#include <type_traits>

class RandomGenerator {
public:
    template<typename T>
    static T get(T min, T max) {
        static std::random_device rd;
        static std::mt19937 generator(rd());
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(generator);
        } else {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(generator);
        }
    }
};
