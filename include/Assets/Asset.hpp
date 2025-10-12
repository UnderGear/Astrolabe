#pragma once

#include <cstdint>

namespace Assets
{
    static constexpr std::int32_t ID_INVALID{ -1 };
}

template <typename ContentT>
struct Asset
{
    ContentT Data;

    std::int32_t ID{ Assets::ID_INVALID };
};
