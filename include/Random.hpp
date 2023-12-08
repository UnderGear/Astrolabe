#pragma once

#include <concepts>
#include <random>
#include <type_traits>

#include "Point.hpp"
#include "Vector.hpp"

template <typename ResultType>
class Random
{ 
    std::mt19937 generator;
    std::uniform_int_distribution<ResultType> distribution;

public:
    explicit Random(ResultType Seed, ResultType Min = std::numeric_limits<ResultType>::lowest(), ResultType Max = std::numeric_limits<ResultType>::max())
        : generator(Seed), distribution(Min, Max)
    { }

    inline void Reset() { distribution.reset(); }

    inline ResultType GetValue() { return distribution(generator); }

    inline Vector2D Vector() requires std::convertible_to<ResultType, std::int32_t>
    {
        return { GetValue(), GetValue() };
    };

    inline Point2D Point() requires std::convertible_to<ResultType, std::int32_t>
    {
        return { GetValue(), GetValue() };
    };
};

//TODO: it might be cool to use a Weibull distribution to weight rolls
