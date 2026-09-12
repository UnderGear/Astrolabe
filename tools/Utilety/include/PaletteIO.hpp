#pragma once

#include <filesystem>
#include <vector>

struct PaletteColor;

namespace PaletteIO
{
	void LoadPalette(std::filesystem::path FileName, std::vector<PaletteColor>& Palette, bool AddAlpha);

	void WritePaletteFile(std::filesystem::path FileName, const std::vector<PaletteColor>& Palette);
}
