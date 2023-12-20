#pragma once

#include <cstdint>

template <typename ContentT>
struct Asset
{
    static constexpr std::int32_t ID_INVALID{ -1 }; //TODO: maybe worth pulling out to a namespace to avoid having one per template type

    ContentT Data;

    std::int32_t ID{ ID_INVALID };
};
