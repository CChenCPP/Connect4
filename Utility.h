#pragma once
#include <random>
#include <chrono>
#include <string>
#include <QString>

typedef unsigned char uint8_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

namespace RNG
{
     static thread_local std::random_device rd;
     static thread_local std::mt19937_64 generator(rd());

     template<typename numType>
     numType randomNum(numType num){
         if (num >= 0) {
             std::uniform_int_distribution<numType> range(0, num);
             return range(generator);
         }
         else {
             std::uniform_int_distribution<numType> range(num, 0);
             return range(generator);
         }
     }

     template<typename numType>
     numType randomNum(numType min, numType max){
         if (max < min) { return 0; };
         std::uniform_int_distribution<numType> range(min, max);
         return range(generator);
     }

     template<typename numType, typename numType2>
     int64_t randomNum(numType min, numType2 max){
         if (max < min) { return 0; };
         std::uniform_int_distribution<long long int> range(min, max);
         return range(generator);
     }

}

namespace Benchmark
{
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

namespace Parse
{
    QString toQString(std::string& str);
}
