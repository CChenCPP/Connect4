#include "Utility.h"

int RNG::randomInt(int max) {
    std::uniform_int_distribution<int> range(0, max);
    return range(generator);
}

int RNG::randomInt(int min, int max) {
    std::uniform_int_distribution<int> range(min, max);
    return range(generator);
}
