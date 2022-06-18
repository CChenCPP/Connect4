#pragma once
#include <random>
#include <chrono>

typedef unsigned char uint8_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

namespace RNG{
     static thread_local std::random_device rd;
     static thread_local std::mt19937 generator(rd());
     int randomInt(int max);
     int randomInt(int min, int max);
}

namespace Benchmark{
    using namespace std::chrono;
    using std::chrono::nanoseconds;
    typedef high_resolution_clock HRC;

    template<typename Function>
    double timeFunctionMs(Function function){
        HRC::time_point begin = HRC::now();
        function();
        HRC::time_point end = HRC::now();
        double elapsedMs = duration_cast<nanoseconds>(end - begin).count() / 1000000.0;
        return elapsedMs;
    }

    template<typename Function>
    double timeFunctionNs(Function function){
        HRC::time_point begin = HRC::now();
        function();
        HRC::time_point end = HRC::now();
        double elapsedNs = duration_cast<nanoseconds>(end - begin).count();
        return elapsedNs;
    }
}
