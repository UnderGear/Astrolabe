#pragma once

#include "Display/SpriteManager.hpp"
#include "TestPalette.hpp"
#include "TestTiles.hpp"

//TODO: figure out the tooling pipeline some more. move Utilety into a subfolder of this repo?

inline constexpr PaletteAsset TestPaletteAsset
{
	std::span<const std::uint32_t>(TestPalette.begin(), TestPalette.end()),
	0x00042
};

inline constexpr SpriteTileAsset TestTileAsset
{
	std::span<const std::uint32_t>{ TestTiles12.begin(), TestTiles12.end() },
	0x00042
};


