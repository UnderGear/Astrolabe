#pragma once

#include <cstdint>

struct TileMapEntry
{
	std::uint16_t TileIndex: 10;
	std::uint16_t HorizontalFlip: 1;
	std::uint16_t VerticalFlip: 1;
	std::uint16_t PaletteBank: 4;
};
