#pragma once

#include <cstdint>

template <typename ContentT>
struct Asset
{
    static constexpr std::int32_t ID_INVALID{ -1 };

    ContentT Data;

    std::int32_t ID{ ID_INVALID };
};
