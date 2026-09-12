// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.

#pragma once

#include <array>
#include <cstdint>
#include <span>

#include "Display/Background.hpp"
#include "Display/BackgroundManager.hpp"

extern const std::array<std::uint32_t, 328> __attribute__((section(".rodata"), aligned(2))) brin_tiles_raw;

extern const std::array<std::uint16_t, 2048> __attribute__((section(".rodata"), aligned(2))) brin_map_raw;

inline constexpr BackgroundTileAsset brin_tiles
{
	std::span<const std::uint32_t>(brin_tiles_raw.begin(), brin_tiles_raw.end()),
	310,
	BackgroundControlRegister::RegularBackgroundDimensions::t64xt32
};

inline constexpr BackgroundMapAsset brin_map
{
	std::span<const std::uint16_t>(brin_map_raw.begin(), brin_map_raw.end()),
	310
};
