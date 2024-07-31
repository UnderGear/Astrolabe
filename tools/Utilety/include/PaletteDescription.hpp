#pragma once

#include <filesystem>
#include <map>

#include "PaletteColor.hpp"

struct PaletteDescription
{
	std::filesystem::path PalettePath;
	std::map<PaletteColor, std::uint16_t> PaletteMapping;
	std::vector<PaletteColor> Palette;
};
