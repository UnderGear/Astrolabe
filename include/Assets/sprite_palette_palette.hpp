// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.

#pragma once

#include <array>
#include <cstdint>
#include <span>

#include "Display/Palette.hpp"

extern const std::array<std::uint32_t, 128> __attribute__((section(".rodata"), aligned(2))) sprite_palette_palette_raw;

static constexpr PaletteAsset sprite_palette_palette
{
	std::span<const std::uint32_t>{ sprite_palette_palette_raw.begin(), sprite_palette_palette_raw.end() }, 155
};
